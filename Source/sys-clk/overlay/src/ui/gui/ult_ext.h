/*
 * Copyright (c) Souldbminer and Horizon OC Contributors
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#pragma once
#include <tesla.hpp>

class ImageElement : public tsl::elm::ListItem {
private:
    const uint8_t* imgData;
    uint32_t imgWidth, imgHeight;
    
public:
    ImageElement(const uint8_t* data, uint32_t w, uint32_t h) 
        : tsl::elm::ListItem(""), imgData(data), imgWidth(w), imgHeight(h) {}
    
    virtual void draw(tsl::gfx::Renderer *renderer) override {
        // Draw image centered horizontally
        u16 centerX = this->getX() + (this->getWidth() - imgWidth) / 2;
        renderer->drawBitmap(
            centerX, 
            this->getY() + 10, 
            imgWidth, 
            imgHeight, 
            imgData
        );
    }
    
    virtual void drawHighlight(tsl::gfx::Renderer *renderer) override {
        // Do nothing - no highlight
    }
    
    virtual bool onClick(u64 keys) override {
        return false; // Non-clickable
    }
    
    virtual Element* requestFocus(Element *oldFocus, tsl::FocusDirection direction) override {
        return nullptr; // Make it non-focusable so it can't be selected
    }
};