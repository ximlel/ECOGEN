//  
//       ,---.     ,--,    .---.     ,--,    ,---.    .-. .-. 
//       | .-'   .' .')   / .-. )  .' .'     | .-'    |  \| | 
//       | `-.   |  |(_)  | | |(_) |  |  __  | `-.    |   | | 
//       | .-'   \  \     | | | |  \  \ ( _) | .-'    | |\  | 
//       |  `--.  \  `-.  \ `-' /   \  `-) ) |  `--.  | | |)| 
//       /( __.'   \____\  )---'    )\____/  /( __.'  /(  (_) 
//      (__)              (_)      (__)     (__)     (__)     
//
//  This file is part of ECOGEN.
//
//  ECOGEN is the legal property of its developers, whose names 
//  are listed in the copyright file included with this source 
//  distribution.
//
//  ECOGEN is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published 
//  by the Free Software Foundation, either version 3 of the License, 
//  or (at your option) any later version.
//  
//  ECOGEN is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with ECOGEN (file LICENSE).  
//  If not, see <http://www.gnu.org/licenses/>.

//! \file      GDDisc.cpp
//! \author    F. Petitpas
//! \version   1.0
//! \date      December 19 2017

#include <vector>
#include "GDDisc.h"
#include <iostream>

using namespace std;
using namespace tinyxml2;

//***************************************************************

GDDisc::GDDisc(string name, vector<Phase*> vecPhases, Mixture *mixture, vector<Transport> vecTransports, XMLElement *element, const int &physicalEntity, string fileName) :
GeometricalDomain(name, vecPhases, mixture, vecTransports, physicalEntity)
{
  XMLElement *sousElement(element->FirstChildElement("dataDisc"));
  if (sousElement == NULL) throw ErrorXMLElement("dataDisc", fileName, __FILE__, __LINE__);
  //Attributes reading
  //------------------
  XMLError error;
  //radius
  error = sousElement->QueryDoubleAttribute("radius", &m_radius);
  if (error != XML_NO_ERROR) throw ErrorXMLAttribut("radius", fileName, __FILE__, __LINE__);
  //Axe1
  string axe(sousElement->Attribute("axe1"));
  Tools::uppercase(axe);
  if (axe == "X"){ m_axe1 = X; }
  else if (axe == "Y"){ m_axe1 = Y; }
  else if (axe == "Z"){ m_axe1 = Z; }
  else { throw ErrorXMLAttribut("axe1", fileName, __FILE__, __LINE__); }
  //Axe2
  axe = sousElement->Attribute("axe2");
  Tools::uppercase(axe);
  if (axe == "X"){ m_axe2 = X; }
  else if (axe == "Y"){ m_axe2 = Y; }
  else if (axe == "Z"){ m_axe2 = Z; }
  else { throw ErrorXMLAttribut("axe2", fileName, __FILE__, __LINE__); }
  //Disc center
  double x(0.), y(0.), z(0.);
  XMLElement *center(sousElement->FirstChildElement("center"));
  if (center == NULL) throw ErrorXMLElement("center", fileName, __FILE__, __LINE__);
  error = center->QueryDoubleAttribute("x", &x);
  error = center->QueryDoubleAttribute("y", &y);
  error = center->QueryDoubleAttribute("z", &z);
  m_centerPos.setXYZ(x, y, z);
}

//***************************************************************

GDDisc::~GDDisc(){}

//***************************************************************

bool GDDisc::belong(Coord &posElement, const int &lvl) const
{
  double sum(0.);
  vector<Axe> axes;
  axes.push_back(m_axe1);
  axes.push_back(m_axe2);

  for (unsigned int i = 0; i < axes.size(); i++)
  {
    switch (axes[i])
    {
    case X:
      sum += pow(posElement.getX() - m_centerPos.getX(), 2.); break;
    case Y:
      sum += pow(posElement.getY() - m_centerPos.getY(), 2.); break;
    case Z:
      sum += pow(posElement.getZ() - m_centerPos.getZ(), 2.); break;
    }
  }
  
  if (sum <= m_radius*m_radius) return true;
  return false;
}