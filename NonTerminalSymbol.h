#pragma once

#ifndef __NON_TERMINAL_SYMBOL__
#define __NON_TERMINAL_SYMBOL__

#include <iostream>
#include <string>
#include <stack>
#include "IInterpretable.h"
#include "PostScriptIntWriter.h"
#include "CGALDefs.h"

/**
  * @class NonTerminalSymbol
  *
  * @brief Parent class for all non-terminal members
  *
  * Parent class of all the \b non-terminal symbols that will
  * be used by the grammar of our L-System. Every Non-Terminal
  * symbol has a replace method that replaces it in the linked
  * list (represented by the next_ pointers) with its child elements ->
  * what the nonterminal turns into.
  *
  * @note No Terminal class exists as NT-Members terminate based on a flag inside
  *
  * @author Lukas Gregori
  * @version $Revision: 1.0 $
  *
  * Contact: lukas.gregori@student.tugraz.at
  *
  *
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
  *
  */
class NonTerminalSymbol {

  public:

	NonTerminalSymbol();
	~NonTerminalSymbol();

	/** @brief Creates new sub-generation of the L-System
	  * 
	  * Replaces the non-terminal symbol by its successors.
	  * Returns the object itself so that it can be freed 
	  * afterwards.
	  *
	  * @return  NonTerminalSymbol* self
	  */
	virtual NonTerminalSymbol* replace() = 0;

	/** @brief Inserts a sublist of NT-Elements
	  *
	  * Inserts a sublist of elements handed as parameter into a second
	  * list specified by the after parameter. Used as each segment replaces
	  * itself in the list
	  *
	  * @param NonTerminalSymbol* sublist Sublist to insert
	  * @param NonTerminalSymbol* after Insert after this element
	  */
	void insertSublist(NonTerminalSymbol* sublist, NonTerminalSymbol* after);

	/// True until symbol can no longer be replaced (Termintates)
	bool terminated_;

	NonTerminalSymbol* next_;
	NonTerminalSymbol* pre_;
};


#endif