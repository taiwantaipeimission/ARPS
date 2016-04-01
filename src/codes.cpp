/* This file is part of ARPS.
*
* (C) 2016 by David B. Elliott (elliott.david.ballantyne@gmail.com)
*
* ARPS is free software: you can redistribute it and/or modify
* it under the terms of Version 3 of the GNU General Public License as published by
* the Free Software Foundation.
*
* ARPS is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ARPS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "codes.h"

map<ReportType, vector<ReportOrder>> ORDERS_TO_TOTAL{
	{ TYPE_REGULAR, { COMP, DISTRICT, DISTRICT_MONTH, ZONE, ZONE_MONTH, WARD, WARD_MONTH, STAKE, STAKE_MONTH, MISSION, MISSION_MONTH } },
	{ TYPE_ENGLISH, { COMP, ZONE, ZONE_MONTH, MISSION } },
	{ TYPE_BAPTISM_RECORD, { COMP } },
	{ TYPE_BAPTISM_SOURCE, { COMP, DISTRICT, DISTRICT_MONTH, ZONE, ZONE_MONTH, WARD, WARD_MONTH, STAKE, STAKE_MONTH, MISSION, MISSION_MONTH } },
	{ TYPE_REFERRAL, { COMP } }
};