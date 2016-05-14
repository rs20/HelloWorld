#include "313178576_B_.h"

REGISTER_ALGORITHM(_313178576_B)


Direction _313178576_B::step(Direction prevStep)
{
	// update robot's position according to the last move that the simulator actually made with the robot
	// as the exercise specifies, the simulator may not work always according to the algorithm's suggested move
	house.updateRobot(prevStep);

	// check if the simulator took the algorithm's advice with the last steps
	if (prevStep != lastMove) {
		// reset all! algorithm will recalculate destination
		returning = false;
		recharging = false;
		goingX = false;
		end = false;
		wayHome.clear();
		xPath.clear();
	}
	// update
	if (prevStep != Direction::Stay)
		lastMove = prevStep;

	// first, if started the move from the docking station -> charge battery
	if (house.getRobot() == house.getDocking())
		curBattery += batteryRechargeRate;
	curBattery = MIN(curBattery, batteryCapacity);

	// update area
	SensorInformation si = sensor->sense();
	house.updateRobotArea(si);

	Direction step;

	// cleaned the whole house -> go to the docking station and stay there
	if (end) {
		if (wayHome.empty())
			step = Direction::Stay;
		else {
			step = wayHome.front();
			wayHome.pop_front();
		}
	}
	// on the way home -> get the first move in the 'wayHome' list
	else if (returning) {
		step = wayHome.front();
		wayHome.pop_front();
		if (wayHome.empty()) {
			returning = false;
			recharging = true;
		}
	}
	// in docking station but needs to recharge more battery before leaving -> stay
	else if (recharging && !goClean()) {
		step = Direction::Stay;
	}
	// didn't have to return to the docking station in the last step and is not recharging atm, but now has to return to the docking station
	else if (!returning && !recharging && goHome()) {
		goingX = false;
		step = wayHome.front();
		wayHome.pop_front();
		if (wayHome.empty())
			recharging = true;
		else
			returning = true;
	}
	// on the way to a new cell with 'X'
	else if (goingX) {
		step = xPath.front();
		xPath.pop_front();
		if (xPath.empty())
			goingX = false;
	}
	// - recharged enough and should clean OR
	// - is not recharging atm, should not go back to the docking station, and should pick a new step
	else {
		recharging = false;
		// (recharging == false, returning == false)
		// clean all dirt, then move to a new cell
		if (si.dirtLevel >= 1)
			step = Direction::Stay;
		else {
			// get the closest cell with 'X' and move to it
			Cell cell = house.getRobot();
			Cell east = { cell.row, cell.col + 1 };
			Cell west = { cell.row, cell.col - 1 };
			Cell south = { cell.row + 1, cell.col };
			Cell north = { cell.row - 1, cell.col };
			// first, check if next to a wall, if so, continue cleaning along the walls
			/*Directon(0)=east,Direction(1)=west,Direction(2)=south,Direction(3)=north,Direction(4)=stay*/
			// example: if east=WALL, and north/south=X, choose south/north
			if (house.hasCell(east) && (house.getCell(east) == 'W') && house.hasCell(south) && (house.getCell(south) == 'X')) {
				step = Direction::South;
			}
			else if (house.hasCell(east) && (house.getCell(east) == 'W') && house.hasCell(north) && (house.getCell(north) == 'X')) {
				step = Direction::North;
			}
			else if (house.hasCell(west) && (house.getCell(west) == 'W') && house.hasCell(south) && (house.getCell(south) == 'X')) {
				step = Direction::South;
			}
			else if (house.hasCell(west) && (house.getCell(west) == 'W') && house.hasCell(north) && (house.getCell(north) == 'X')) {
				step = Direction::North;
			}
			else if (house.hasCell(south) && (house.getCell(south) == 'W') && house.hasCell(east) && (house.getCell(east) == 'X')) {
				step = Direction::East;
			}
			else if (house.hasCell(south) && (house.getCell(south) == 'W') && house.hasCell(west) && (house.getCell(west) == 'X')) {
				step = Direction::West;
			}
			else if (house.hasCell(north) && (house.getCell(north) == 'W') && house.hasCell(east) && (house.getCell(east) == 'X')) {
				step = Direction::East;
			}
			else if (house.hasCell(north) && (house.getCell(north) == 'W') && house.hasCell(west) && (house.getCell(west) == 'X')) {
				step = Direction::West;
			}
			// else (no walls) clean immediate dirt around you
			else if (house.hasCell(east) && (house.getCell(east) > '0') && (house.getCell(east) <= '9'))
				step = Direction::East;
			else if (house.hasCell(west) && (house.getCell(west) > '0') && (house.getCell(west) <= '9'))
				step = Direction::West;
			else if (house.hasCell(south) && (house.getCell(south) > '0') && (house.getCell(south) <= '9'))
				step = Direction::South;
			else if (house.hasCell(north) && (house.getCell(north) > '0') && (house.getCell(north) <= '9'))
				step = Direction::North;
			// then go to immediate X around you
			else if (house.hasCell(east) && (house.getCell(east) == 'X'))
				step = Direction::East;
			else if (house.hasCell(west) && (house.getCell(west) == 'X'))
				step = Direction::West;
			else if (house.hasCell(south) && (house.getCell(south) == 'X'))
				step = Direction::South;
			else if (house.hasCell(north) && (house.getCell(north) == 'X'))
				step = Direction::North;
			// then search for the closest X / dirt
			else {
				// no immediate 'X' cells around robot
				// run BFS to get 'path' updated to hold shortest path to the closest 'X' or '1'-'9' in the house
				// note that we may find '1'-'9' cells (actually 9 is not possible) in case we had to go back
				// to the docking station while cleaning a cell
				Cell xCell;
				xPath = std::move(house.BFS(house.getRobot(), xCell, 'X'));
				if (xPath.empty()) {
					end = true;
					// no 'X' was found -> cleaned the whole house! go Home!
					// BFS (shortest path to the docking station)
					Cell homeCell;
					wayHome = std::move(house.BFS(house.getRobot(), homeCell, 'D'));
					step = wayHome.front();
					wayHome.pop_front();
				}
				else {
					// check if has enough battery to get to the 'X', if not -> go to the Docking station
					Cell homeCell;
					wayHome = std::move(house.BFS(xCell, homeCell, 'D'));
					int distance = xPath.size() + wayHome.size();
					wayHome.clear();
					int movesToMake = curBattery / batteryConsumptionRate;
					if (moreSteps != -1)
						movesToMake = MIN(movesToMake, moreSteps);
					// not enough to 'X' + home => just go home
					if (distance >= movesToMake - 2) {
						xPath.clear();
						wayHome = std::move(house.BFS(house.getRobot(), homeCell, 'D'));
						// in case already in D and the X is impossible to reach -> stay
						if (wayHome.empty()) {
							end = true;
							step = Direction::Stay;
						}
						else {
							step = wayHome.front();
							wayHome.pop_front();
							returning = true;
							if (wayHome.empty()) {
								returning = false;
								recharging = true;
							}
						}
					}
					else {
						step = xPath.front();
						xPath.pop_front();
						if (!xPath.empty())
							goingX = true;
					}
				}
			}
		}
	}

	// consume battery only if did not start the move from the docking station
	if (house.getRobot() != house.getDocking())
		curBattery -= batteryConsumptionRate;

	// do not update spot according to 'step' chosen because simulator may not choose the algorithm's suggested step!
	// updated only at start of this method (sense) -> the simulator passes the actual step that has been made
	lastMove = step;

	if (moreSteps != -1)
		moreSteps--;
	return step;
}