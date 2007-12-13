// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2006, 2007 Elliot Glaysher
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  
// -----------------------------------------------------------------------

#include "Precompiled.hpp"

// -----------------------------------------------------------------------

#include "Systems/Base/GraphicsObject.hpp"
#include "Systems/Base/GraphicsObjectData.hpp"
#include "Utilities.h"

#include <iostream>
#include <algorithm>
#include <numeric>

using namespace std;

const int DEFAULT_TEXT_SIZE = 14;
const int DEFAULT_TEXT_XSPACE = 0;
const int DEFAULT_TEXT_YSPACE = 0;
const int DEFAULT_TEXT_VERTICAL = 0;
const int DEFAULT_TEXT_COLOUR = 0;
const int DEFAULT_TEXT_SHADOWCOLOUR = 0;

// -----------------------------------------------------------------------
// GraphicsObject::TextProperties
// -----------------------------------------------------------------------
GraphicsObject::TextProperties::TextProperties()
  : textSize(DEFAULT_TEXT_SIZE), 
    xspace(DEFAULT_TEXT_XSPACE), 
    yspace(DEFAULT_TEXT_YSPACE), 
    vertical(DEFAULT_TEXT_VERTICAL),
    colour(DEFAULT_TEXT_COLOUR),
    shadowColour(DEFAULT_TEXT_SHADOWCOLOUR)
{
}

// -----------------------------------------------------------------------
// GraphicsObject
// -----------------------------------------------------------------------

GraphicsObject::GraphicsObject()
  : m_visible(false), 
    m_x(0), m_y(0), 
    m_whateverAdjustVertOperatesOn(0),
    m_originX(0), m_originY(0),
    m_repOriginX(0), m_repOriginY(0),

    // Width and height are percentages
    m_width(100), m_height(100), 

    m_rotation(0),

    m_pattNo(0), m_alpha(255),

    m_clipX1(0), m_clipY1(0), m_clipX2(-1), m_clipY2(-1),

    m_mono(0), m_invert(0),
    // Do the rest later.
    m_tintR(255), m_tintG(255), m_tintB(255), 
    m_colourR(255), m_colourG(255), m_colourB(255), m_colourLevel(255),
    m_compositeMode(0),

    m_wipeCopy(0)
{
  // Regretfully, we can't do this in the initializer list.
  fill(m_adjustX, m_adjustX + 8, 0);
  fill(m_adjustY, m_adjustY + 8, 0);
}

// -----------------------------------------------------------------------

GraphicsObject::GraphicsObject(const GraphicsObject& rhs)
  : m_visible(rhs.m_visible), m_x(rhs.m_x), m_y(rhs.m_y),
    m_whateverAdjustVertOperatesOn(rhs.m_whateverAdjustVertOperatesOn),
    m_originX(rhs.m_originX), m_originY(rhs.m_originY), 
    m_repOriginX(rhs.m_repOriginX), m_repOriginY(rhs.m_repOriginY),
    m_width(rhs.m_width), m_rotation(rhs.m_rotation),
    m_pattNo(rhs.m_pattNo), m_alpha(rhs.m_alpha),
    m_clipX1(rhs.m_clipX1), m_clipY1(rhs.m_clipY1),
    m_clipX2(rhs.m_clipX2), m_clipY2(rhs.m_clipY2),
    m_mono(rhs.m_mono), m_invert(rhs.m_invert),
    m_light(rhs.m_light), m_tintR(rhs.m_tintR),
    m_tintG(rhs.m_tintG), m_tintB(rhs.m_tintB),
    m_colourR(rhs.m_colourR), m_colourG(rhs.m_colourG),
    m_colourB(rhs.m_colourB), m_colourLevel(rhs.m_colourLevel),
    m_compositeMode(rhs.m_compositeMode),
    m_scrollRateX(rhs.m_scrollRateX),
    m_scrollRateY(rhs.m_scrollRateY), 

	m_wipeCopy(0)
{   
  if(rhs.m_textProperties)
    m_textProperties.reset(new TextProperties(*rhs.m_textProperties));

  if(rhs.m_objectData)
    m_objectData.reset(rhs.m_objectData->clone());

  copy(rhs.m_adjustX, rhs.m_adjustX + 8, m_adjustX);
  copy(rhs.m_adjustY, rhs.m_adjustY + 8, m_adjustY);
}

// -----------------------------------------------------------------------

GraphicsObject::~GraphicsObject()
{}

// -----------------------------------------------------------------------

GraphicsObject& GraphicsObject::operator=(const GraphicsObject& rhs)
{
  if(this != &rhs)
  {
//    cerr << "Copying graphics object!" << endl;

    m_visible = rhs.m_visible;
    m_x = rhs.m_x;
    m_y = rhs.m_y;

    copy(rhs.m_adjustX, rhs.m_adjustX + 8, m_adjustX);
    copy(rhs.m_adjustY, rhs.m_adjustY + 8, m_adjustY);

    m_whateverAdjustVertOperatesOn = rhs.m_whateverAdjustVertOperatesOn;
    m_originX = rhs.m_originX;
    m_originY = rhs.m_originY;
    m_repOriginX = rhs.m_repOriginX;
    m_repOriginY = rhs.m_repOriginY;
    m_width = rhs.m_width;
    m_height = rhs.m_height;
    m_rotation = rhs.m_rotation;

    m_pattNo = rhs.m_pattNo;
    m_alpha = rhs.m_alpha;
    m_clipX1 = rhs.m_clipX1; m_clipY1 = rhs.m_clipY1;
    m_clipX2 = rhs.m_clipX2; m_clipY2 = rhs.m_clipY2;
    m_mono = rhs.m_mono;
    m_invert = rhs.m_invert;
    m_light = rhs.m_light;
    m_tintR = rhs.m_tintR;
    m_tintG = rhs.m_tintG;
    m_tintB = rhs.m_tintB;

    m_colourR = rhs.m_colourR;
    m_colourG = rhs.m_colourG;
    m_colourB = rhs.m_colourB;
    m_colourLevel = rhs.m_colourLevel;

    m_compositeMode = rhs.m_compositeMode;
    m_scrollRateX = rhs.m_scrollRateX;
    m_scrollRateY = rhs.m_scrollRateY;

    if(rhs.m_textProperties)
      m_textProperties.reset(new TextProperties(*rhs.m_textProperties));

    m_wipeCopy = rhs.m_wipeCopy;
      
    if(rhs.m_objectData)
    {
      m_objectData.reset(rhs.m_objectData->clone());
      m_objectData->setOwnedBy(*this);
    }
  }

  return *this;
}

// -----------------------------------------------------------------------

void GraphicsObject::setObjectData(GraphicsObjectData* obj)
{
  m_objectData.reset(obj);
  m_objectData->setOwnedBy(*this);
}


// -----------------------------------------------------------------------

int GraphicsObject::xAdjustmentSum() const 
{
  return std::accumulate(m_adjustX, m_adjustX + 8, 0);
}

// -----------------------------------------------------------------------

int GraphicsObject::yAdjustmentSum() const
{ 
  return std::accumulate(m_adjustY, m_adjustY + 8, 0); 
}

// -----------------------------------------------------------------------

int GraphicsObject::pixelWidth(RLMachine& machine) const
{
  // Calculate out the pixel width of the current object taking in the
  // width() scaling.
  if(hasObjectData())
    return m_objectData->pixelWidth(machine, *this);
  else
    return 0;
}

// -----------------------------------------------------------------------

void GraphicsObject::setAlpha(const int alpha)
{   
//  cerr << "m_alpha: " << alpha << endl;
  m_alpha = alpha;
}

// -----------------------------------------------------------------------

int GraphicsObject::pixelHeight(RLMachine& machine) const
{
  if(hasObjectData())
    return m_objectData->pixelHeight(machine, *this);
  else
    return 0;
}

// -----------------------------------------------------------------------

GraphicsObjectData& GraphicsObject::objectData() const 
{
  if(m_objectData)
    return *m_objectData;
  else
  {
    throw rlvm::Exception("null object data");
  }
}

// -----------------------------------------------------------------------

GraphicsObjectData* GraphicsObject::objectDataPtr() const 
{
  return m_objectData.get();
}

// -----------------------------------------------------------------------

void GraphicsObject::setTextText(const std::string& utf8str) {
  makeSureHaveTextProperties();
  m_textProperties->value = utf8str; 
}

// -----------------------------------------------------------------------

const std::string& GraphicsObject::textText() const { 
  static const std::string empty = "";

  if(m_textProperties)
    return m_textProperties->value; 
  else
    return empty;
}

// -----------------------------------------------------------------------

int GraphicsObject::textSize() const {
  if(m_textProperties)
    return m_textProperties->textSize; 
  else
    return DEFAULT_TEXT_SIZE;
}

// -----------------------------------------------------------------------

int GraphicsObject::textXSpace() const {
  if(m_textProperties)
    return m_textProperties->xspace; 
  else
    return DEFAULT_TEXT_XSPACE;
}

// -----------------------------------------------------------------------

int GraphicsObject::textYSpace() const {
  if(m_textProperties)
    return m_textProperties->yspace; 
  else
    return DEFAULT_TEXT_YSPACE;
}

// -----------------------------------------------------------------------

int GraphicsObject::textVertical() const {
  if(m_textProperties)
    return m_textProperties->vertical; 
  else
    return DEFAULT_TEXT_VERTICAL;
}

// -----------------------------------------------------------------------

int GraphicsObject::textColour() const {
  if(m_textProperties)
    return m_textProperties->colour; 
  else
    return DEFAULT_TEXT_COLOUR;
}

// -----------------------------------------------------------------------

int GraphicsObject::textShadowColour() const {
  if(m_textProperties)
    return m_textProperties->shadowColour; 
  else
    return DEFAULT_TEXT_SHADOWCOLOUR;
}

// -----------------------------------------------------------------------

void GraphicsObject::makeSureHaveTextProperties()
{
  if(!m_textProperties)
    m_textProperties.reset(new TextProperties());
}

// -----------------------------------------------------------------------

void GraphicsObject::setTextOps(
  int size, int xspace, int yspace, int vertical, int colour, int shadow)
{
  makeSureHaveTextProperties();
  m_textProperties->textSize = size;
  m_textProperties->xspace = xspace;
  m_textProperties->yspace = yspace;
  m_textProperties->vertical = vertical;
  m_textProperties->colour = colour;
  m_textProperties->shadowColour = shadow;
}

// -----------------------------------------------------------------------

void GraphicsObject::render(RLMachine& machine)
{
//  bool needNewline = false;
  
//   if(m_objectData) {
//     cerr << "Has object data...";
//     needNewline = true;
//   }

//   if(visible()) {
//     cerr << "Is visible...";
//     needNewline = true;
//   }

//   if(needNewline)
//     cerr << endl;
      
  if(m_objectData && visible()) {
//    cerr << "Rendering object!" << endl;
    m_objectData->render(machine, *this);
  }
//  else {
//    cerr << "NOT rendering object!" << endl;
//  }
}

// -----------------------------------------------------------------------

void GraphicsObject::deleteObject()
{
  m_objectData.reset();
}

// -----------------------------------------------------------------------

void GraphicsObject::clearObject()
{
  *this = GraphicsObject();
}

// -----------------------------------------------------------------------

void GraphicsObject::execute(RLMachine& machine)
{
  if(m_objectData) {
    m_objectData->execute(machine);
  }
}

// -----------------------------------------------------------------------

void GraphicsObject::setCompositeMode(const int in) 
{ 
  m_compositeMode = in; 
}
