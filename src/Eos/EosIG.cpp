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

//! \file      EosIG.cpp
//! \author    F. Petitpas, K. Schmidmayer, E. Daniel
//! \version   1.0
//! \date      May 14 2018

#include <cmath>
#include <algorithm>
#include <cstring>
#include "EosIG.h"

using namespace std;

//***********************************************************************

EosIG::EosIG(){}

//***********************************************************************

EosIG::EosIG(vector<string> &nameParameterEos, int &number) :
    Eos(number)
{
  nameParameterEos.push_back("gamma");
  nameParameterEos.push_back("cv");
  nameParameterEos.push_back("energyRef");
  nameParameterEos.push_back("entropyRef");
}

//***********************************************************************

EosIG::~EosIG(){}

//***********************************************************************

//Mod
void EosIG::sendInfo(double *&data) const
{
	int nameber = 3;
	data = new double[nameber];

	data[0] = m_gamma;
	data[1] = m_cv;
	data[2] = m_eRef;
}

//***********************************************************************

void EosIG::assignParametersEos(string name, vector<double> parametersEos)
{
  m_name   = name;
  assert(parametersEos.size() == 4);
  m_gamma = parametersEos[0];
  m_cv    = parametersEos[1];
  m_eRef  = parametersEos[2];
  m_sRef  = parametersEos[3];
}

//***********************************************************************

//Constant methods
//****************
double EosIG::computeTemperature(const double &density, const double &pressure) const
{
  return pressure/(m_gamma-1.)/max(density, epsilon)/m_cv;
}

//***********************************************************************

double EosIG::computeEnergy(const double &density, const double &pressure) const
{
  return pressure/ (m_gamma-1.)/max(density, epsilon) + m_eRef;
}

//***********************************************************************

double EosIG::computePressure(const double &density, const double &energy) const
{
  return (m_gamma-1.)*density*(energy-m_eRef);
}

//***********************************************************************

double EosIG::computeDensity(const double &pressure, const double &temperature) const
{
  return pressure/max(((m_gamma - 1.)*m_cv*temperature), epsilon);
}

//***********************************************************************

double EosIG::computeSoundSpeed(const double &density, const double &pressure) const
{
  return sqrt(m_gamma*pressure/max(density, epsilon));
}

//***********************************************************************

double EosIG::computeEntropy(const double &temperature, const double &pressure) const
{
  return m_cv*log(pow(temperature, m_gamma) / max(pow(pressure, m_gamma - 1.), epsilon)) + m_sRef;
}

//***********************************************************************

double EosIG::computePressureIsentropic(const double &initialPressure, const double &initialDensity, const double &finalDensity) const
{
  return initialPressure*pow(finalDensity/max(initialDensity, epsilon),m_gamma);
}

//***********************************************************************

double EosIG::computePressureHugoniot(const double &initialPressure, const double &initialDensity, const double &finalDensity) const
{
  return initialPressure*((m_gamma+1.)*finalDensity-(m_gamma-1.)*initialDensity)/max(((m_gamma+1.)*initialDensity-(m_gamma-1.)*finalDensity), epsilon);
}

//***********************************************************************

double EosIG::computeDensityIsentropic(const double &initialPressure, const double &initialDensity, const double &finalPressure, double *drhodp) const
{
  double finalDensity(initialDensity*pow(finalPressure/max(initialPressure, epsilon),1./m_gamma));
  if(drhodp!=NULL) *drhodp = finalDensity/max((m_gamma*finalPressure), epsilon);
  return finalDensity;
}

//***********************************************************************

double EosIG::computeDensityHugoniot(const double &initialPressure, const double &initialDensity, const double &finalPressure, double *drhodp) const
{
  double num((m_gamma + 1.)*finalPressure + (m_gamma - 1.)*initialPressure);
  double denom((m_gamma - 1.)*finalPressure + (m_gamma + 1.)*initialPressure);
  double finalDensity(initialDensity*num / max(denom, epsilon));
  if (drhodp != NULL) *drhodp = initialDensity*4.*m_gamma*initialPressure / max((denom*denom), epsilon);
  return finalDensity;
}

//***********************************************************************

double EosIG::computeDensityPfinal(const double &initialPressure, const double &initialDensity, const double &finalPressure, double *drhodp) const
{
  double num((m_gamma)*finalPressure);
  double denom(num + initialPressure - finalPressure);
  double finalDensity(initialDensity*num / max(denom, epsilon));
  if (drhodp != NULL) *drhodp = initialDensity*m_gamma*initialPressure / max((denom*denom), epsilon);
  return finalDensity;
}

//***********************************************************************

double EosIG::computeEnthalpyIsentropic(const double &initialPressure, const double &initialDensity, const double &finalPressure, double *dhdp) const
{
  double finalRho, drho;
  finalRho = this->computeDensityIsentropic(initialPressure, initialDensity, finalPressure, &drho);
  double finalEnthalpy(m_gamma*finalPressure/(m_gamma-1.)/max(finalRho, epsilon) +m_eRef);
  if (dhdp != NULL) *dhdp = m_gamma / (m_gamma - 1.)*(finalRho - finalPressure*drho) / max((finalRho*finalRho), epsilon);
  return finalEnthalpy;
}

//***********************************************************************

double EosIG::computeDensitySaturation(const double &pressure, const double &Tsat, const double &dTsatdP, double *drhodp) const
{
  double rho;
  if (drhodp != NULL) {
    *drhodp = (m_gamma - 1.)*m_cv*Tsat - pressure*(m_gamma - 1.)*m_cv*dTsatdP;
    *drhodp /= max((((m_gamma - 1.)*m_cv*Tsat)*((m_gamma - 1.)*m_cv*Tsat)), epsilon);
  }
  rho = pressure/max(((m_gamma - 1.)*m_cv*Tsat), epsilon);
  return rho;
}

//***********************************************************************

double EosIG::computeDensityEnergySaturation(const double &pressure, const double &rho, const double &drhodp, double *drhoedp) const
{
  double rhoe;
  if (drhoedp != NULL) {  *drhoedp = 1. / (m_gamma - 1.) + drhodp*m_eRef; }
  rhoe = pressure / (m_gamma - 1.) + rho*m_eRef;
  return rhoe;
}

//***********************************************************************

void EosIG::sendSpecialMixtureEos(double &gamPinfOverGamMinusOne, double &eRef, double &oneOverGamMinusOne) const
{
  gamPinfOverGamMinusOne = 0.;
  eRef = m_eRef;
  oneOverGamMinusOne = 1. / (m_gamma - 1.);
}

//***********************************************************************

double EosIG::vfpfh(const double &pressure, const double &enthalpy) const
{
  return (m_gamma - 1.)*(enthalpy - m_eRef) / max((m_gamma*pressure), epsilon);
}

//***********************************************************************

double EosIG::dvdpch(const double &pressure, const double &enthalpy) const
{
  return (1. - m_gamma) / m_gamma * (enthalpy - m_eRef) / max((pressure*pressure), epsilon);
}

//***********************************************************************

double EosIG::dvdhcp(const double &pressure, const double &enthalpy) const
{
  return (m_gamma - 1.) / m_gamma / max(pressure, epsilon);
}

//***********************************************************************

void EosIG::verifyPressure(const double &pressure, const std::string &message) const
{
  if (pressure < 1.e-15) errors.push_back(Errors(message + " : too low pressure in EosIG"));
}

//***********************************************************************

void EosIG::verifyAndModifyPressure(double &pressure) const
{
  if (pressure < 1.e-15) pressure = 1.e-15;
}

//***********************************************************************

double EosIG::getGamma() const { return m_gamma; }

//***********************************************************************

double EosIG::getCv() const { return m_cv; }

//***********************************************************************

double EosIG::getERef() const { return m_eRef; }

//***********************************************************************

double EosIG::getSRef() const { return m_sRef; }


//***********************************************************************