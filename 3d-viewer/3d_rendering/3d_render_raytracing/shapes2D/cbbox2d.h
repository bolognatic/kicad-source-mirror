/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015-2016 Mario Luzeiro <mrluzeiro@ua.pt>
 * Copyright (C) 1992-2020 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

 /**
 * @file cbbox2d.h
 * @brief 2D Bounding Box class definition
 */

#ifndef _CBBOX2D_H_
#define _CBBOX2D_H_

#include "../ray.h"


/**
 * Manage a bounding box defined by two SFVEC2F min max points.
 */
struct CBBOX2D
{

public:

    /**
     * Create with default values a bounding box (not initialized).
     */
    CBBOX2D();

    /**
     * Initialize a bounding box with a given point.
     *
     * @param aPbInit a point for the bounding box initialization.
     */
    explicit CBBOX2D( const SFVEC2F &aPbInit );

    /**
     * Initialize a bounding box with a minimum and a maximum point.
     *
     * @param aPbMin the minimum point to initialize the bounding box.
     * @param aPbMax the maximum point to initialize the bounding box.
     */
    CBBOX2D( const SFVEC2F &aPbMin, const SFVEC2F &aPbMax );

    ~CBBOX2D();


    /**
     * Set bounding box with new parameters.
     *
     * @param aPbMin the minimum point to initialize the bounding box.
     * @param aPbMax the maximum point to initialize the bounding box.
     */
    void Set( const SFVEC2F &aPbMin, const SFVEC2F &aPbMax );

    /**
     * Set bounding box based on another bounding box.
     *
     * @param CBBOX2D a bounding box to initialize this one.
     */
    void Set( const CBBOX2D &aBBox );

    /**
     * Recalculate the bounding box adding a point.
     *
     * @param aPoint the point to be bounded
     */
    void Union( const SFVEC2F &aPoint );

    /**
     * Recalculate the bounding box adding other bounding box.
     *
     * @param aBBox the bounding box to be bounded.
     */
    void Union( const CBBOX2D &aBBox );

    /**
     * Scale a bounding box by its center.
     *
     * @param aScale scale factor to apply
     */
    void Scale( float aScale );

    /**
     * Scale a bounding box to the next float representation making it larger.
     */
    void ScaleNextUp();

    /**
     * Scale a bounding box to the next float representation making it smaller.
     */
    void ScaleNextDown();

    /**
     * Test if a bounding box intersects this box.
     *
     * @param aBBox the bounding box to check if it intersects.
     */
    bool Intersects( const CBBOX2D &aBBox ) const;

    /**
     * Test if a circle intersects this box.
     *
     * @param aBBox the bounding box to check if it intersects.
     */
    bool Intersects( const SFVEC2F &aCenter, float aRadiusSquared ) const;

    /**
     * Check is a point is inside this bounding box.
     *
     * @param aPoint point to test.
     */
    bool Inside( const SFVEC2F &aPoint ) const;

    /**
     * Calculate the area of a bounding box.
     *
     * @return area of this bounding box.
     */
    float Area() const;

    /**
     * Check if this bounding box is already initialized.
     *
     * @return True if it was initialized, otherwise false.
     */
    bool IsInitialized() const;

    /**
     * Reset the bounding box to zero and uninitialize it.
     */
    void Reset();

    /**
     * Return the center point of the bounding box.
     *
     * @return the position of the center of this bounding box.
     */
    SFVEC2F GetCenter() const;

    /**
     * @return SFVEC2F - max-min
     */
    SFVEC2F GetExtent() const;

    /**
     * Return the minimum vertex point.
     *
     * @return the minimum vertex position.
     */
    const SFVEC2F &Min() const { return m_min; }

    /**
     * Return the maximum vertex point.
     * @return the maximum vertex position.
     */
    const SFVEC2F &Max() const { return m_max; }


    /**
     * @return the index of the max dimension (0=x, 1=y)
     */
    unsigned int MaxDimension() const;

    /**
     * @return the surface area of the box
     */
    float Perimeter() const;

    /**
     * @param aRay ray to intersect the box
     * @param t distance point of the ray of the intersection (if true)
     * @return true if the ray hits the box
     */
    bool Intersect( const RAY2D &aRay, float *t ) const;

    bool Intersect( const RAY2D &aRay, float *aOutHitT0, float *aOutHitT1 ) const;

    bool Intersect( const RAYSEG2D &aRaySeg ) const;

private:
    SFVEC2F m_min; ///< point of the lower position of the bounding box
    SFVEC2F m_max; ///< point of the higher position of the bounding box
};

#endif // CBBox2d_h
