/*
 * -----------------------------------------------------------------
 * Copyright (C) Lukas Gregori, contact@lukasgregori.com
 *
 * This file is part of the road network generator RoadGen.
 *
 * RoadGen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * RoadGen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * -----------------------------------------------------------------
*/

#include "NonTerminalSymbol.h"

//---------------------------------------------------------------------------
NonTerminalSymbol::NonTerminalSymbol() {
	next_ = 0x0;
	pre_ = 0x0;
	terminated_ = false;
}

//---------------------------------------------------------------------------
NonTerminalSymbol::~NonTerminalSymbol() {
}

//---------------------------------------------------------------------------
void NonTerminalSymbol::insertSublist(NonTerminalSymbol* sublist, NonTerminalSymbol* after) {
	NonTerminalSymbol* last = after->next_;

	after->next_ = sublist;
	sublist->pre_ = after;

	NonTerminalSymbol* tmp = sublist;
	while (tmp->next_ != 0x0)
		tmp = tmp->next_;
	tmp->next_ = last;

	if(last != 0x0)
		last->pre_ = tmp;
}
