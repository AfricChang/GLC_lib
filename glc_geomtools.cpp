/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.9, packaged on May, 2008.

 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

//! \file glc_geomtools.cpp implementation of geometry function.

#include "glc_geomtools.h"
#include "glc_maths.h"

//////////////////////////////////////////////////////////////////////
//Tools Functions
//////////////////////////////////////////////////////////////////////

// test if a polygon of mesh is convex
bool glc::polygonIsConvex(const GLC_Mesh2* pMesh, const QVector<int>& vertexs)
{
	const int max= vertexs.size();
	if (max < 4) return true;
	
	GLC_Vector4d vertex1(pMesh->getVertex(vertexs[0]));
	GLC_Vector4d vertex2(pMesh->getVertex(vertexs[1]));
	GLC_Vector4d edge1(vertex2 - vertex1);
	
	vertex1= vertex2;
	vertex2= pMesh->getVertex(vertexs[2]);
	GLC_Vector4d edge2(vertex2 - vertex1);
	
	const bool direction= (edge1 ^ edge2).getZ() >= 0.0;
	
	for (int i= 3; i < max; ++i)
	{
		edge1= edge2;
		vertex1= vertex2;
		vertex2= pMesh->getVertex(vertexs[i]);
		edge2= vertex2 - vertex1;
		if (((edge1 ^ edge2).getZ() >= 0.0) != direction)
			return false;		
	}
	// The last edge with the first
	edge1= edge2;
	vertex1= vertex2;
	vertex2= pMesh->getVertex(vertexs[0]);
	edge2= vertex2 - vertex1;
	if (((edge1 ^ edge2).getZ() >= 0.0) != direction)
		return false;		
	
	// Ok, the polygon is convexe
	return true;
}
// find intersection between two 2D segments
QVector<GLC_Vector2d> glc::findIntersection(const GLC_Vector2d& s1p1, const GLC_Vector2d& s1p2, const GLC_Vector2d& s2p1, const GLC_Vector2d& s2p2)
{
	const GLC_Vector2d D0= s1p2 - s1p1;
	const GLC_Vector2d D1= s2p2 - s2p1;
	// The QVector of result points
	QVector<GLC_Vector2d> result;
	
	const GLC_Vector2d E(s2p1 - s1p1);
	double kross= D0 ^ D1;
	double sqrKross= kross * kross;
	const double sqrLen0= D0.getX() * D0.getX() + D0.getY() * D0.getY();
	const double sqrLen1= D1.getX() * D1.getX() + D1.getY() * D1.getY();
	// Test if the line are nor parallel
	if (sqrKross > (EPSILON * sqrLen0 * sqrLen1))
	{
		const double s= (E ^ D1) / kross;
		if ((s < 0.0) or (s > 1.0))
		{
			// Intersection of lines is not a point on segment s1p1 + s * DO
			return result; // Return empty QVector
		}
		const double t= (E ^ D0) / kross;
		
		if ((t < 0.0) or (t > 1.0))
		{
			// Intersection of lines is not a point on segment s2p1 + t * D1
			return result; // Return empty QVector
		}
		
		// Intersection of lines is a point on each segment
		result << (s1p1 + (D0 * s));
		return result; // Return a QVector of One Point
	}
	
	// Lines of the segments are parallel
	const double sqrLenE= E.getX() * E.getX() + E.getY() * E.getY();
	kross= E ^ D0;
	sqrKross= kross * kross;
	if (sqrKross > (EPSILON * sqrLen0 * sqrLenE))
	{
		// Lines of the segments are different
		return result; // Return empty QVector
	}
	
	// Lines of the segments are the same. Need to test for overlap of segments.
	const double s0= (D0 * E) / sqrLen0;
	const double s1= (D0 * D1) / sqrLen0;
	const double sMin= fmin(s0, s1);
	const double sMax= fmax(s0, s1);
	QVector<double> overlaps= findIntersection(0.0, 1.0, sMin, sMax);
	const int iMax= overlaps.size();
	for (int i= 0; i < iMax; ++i)
	{
		result.append(s1p1 + (D0 * overlaps[i]));
	}
	return result;
}

// return true if there is an intersection between 2 segments
bool glc::isIntersected(const GLC_Vector2d& s1p1, const GLC_Vector2d& s1p2, const GLC_Vector2d& s2p1, const GLC_Vector2d& s2p2)
{
	const GLC_Vector2d D0= s1p2 - s1p1;
	const GLC_Vector2d D1= s2p2 - s2p1;
	
	const GLC_Vector2d E(s2p1 - s1p1);
	double kross= D0 ^ D1;
	double sqrKross= kross * kross;
	const double sqrLen0= D0.getX() * D0.getX() + D0.getY() * D0.getY();
	const double sqrLen1= D1.getX() * D1.getX() + D1.getY() * D1.getY();
	// Test if the line are nor parallel
	if (sqrKross > (EPSILON * sqrLen0 * sqrLen1))
	{
		const double s= (E ^ D1) / kross;
		if ((s < 0.0) or (s > 1.0))
		{
			// Intersection of lines is not a point on segment s1p1 + s * DO
			return false;
		}
		const double t= (E ^ D0) / kross;
		
		if ((t < 0.0) or (t > 1.0))
		{
			// Intersection of lines is not a point on segment s2p1 + t * D1
			return false;
		}
		
		// Intersection of lines is a point on each segment
		return true;
	}
	
	// Lines of the segments are parallel
	const double sqrLenE= E.getX() * E.getX() + E.getY() * E.getY();
	kross= E ^ D0;
	sqrKross= kross * kross;
	if (sqrKross > (EPSILON * sqrLen0 * sqrLenE))
	{
		// Lines of the segments are different
		return false;
	}
	
	// Lines of the segments are the same. Need to test for overlap of segments.
	const double s0= (D0 * E) / sqrLen0;
	const double s1= s0 + (D0 * D1) / sqrLen0;
	const double sMin= fmin(s0, s1);
	const double sMax= fmax(s0, s1);
	if (findIntersection(0.0, 1.0, sMin, sMax).size() == 0) return false; else return true;
	
}

// find intersection of two intervals [u0, u1] and [v0, v1]
QVector<double> glc::findIntersection(const double& u0, const double& u1, const double& v0, const double& v1)
{
	Q_ASSERT((u0 < u1) and (v0 < v1));
	QVector<double> result;
	if (u1 < v0 or u0 > v1) return result; // Return empty QVector
	
	if (u1 > v0)
	{
		if (u0 < v1)
		{
			if (u0 < v0) result.append(v0); else result.append(u0);
			if (u1 > v1) result.append(v1); else result.append(u1);
			return result;
		}
		else // u0 == v1
		{
			result.append(u0);
			return result;
		}
	}
	else // u1 == v0
	{
		result.append(u1);
		return result;
	}
}

// return true if the segment is in polygon cone
bool glc::segmentInCone(const GLC_Vector2d& V0, const GLC_Vector2d& V1, const GLC_Vector2d& VM, const GLC_Vector2d& VP)
{
	// assert: VM, V0, VP are not collinear
	const GLC_Vector2d diff(V1 - V0);
	const GLC_Vector2d edgeL(VM - V0);
	const GLC_Vector2d edgeR(VP - V0);
	if ((edgeR ^ edgeL) < 0)
	{
		// Vertex is convex
		return (((diff ^ edgeR) > 0.0) and ((diff ^ edgeL) < 0.0));
	}
	else
	{
		// Vertex is reflex
		return (((diff ^ edgeR) > 0.0) or ((diff ^ edgeL) < 0.0));
	}
}

// Return true if the segment is a polygon diagonal
bool glc::isDiagonal(const QList<GLC_Vector2d>& polygon, const int i0, const int i1)
{
	const int size= polygon.size();
	int iM= (i0 - 1) % size;
	if (iM < 0) iM= size - 1;
	int iP= (i0 + 1) % size;

	if (not segmentInCone(polygon[i0], polygon[i1], polygon[iM], polygon[iP]))
	{
		return false;
	}
	
	int j0= 0;
	int j1= size - 1;
	// test segment <polygon[i0], polygon[i1]> to see if it is a diagonal
	while (j0 < size)
	{
		if (j0 != i0 && j0 != i1 && j1 != i0 && j1 != i1)
		{
			if (isIntersected(polygon[i0], polygon[i1], polygon[j0], polygon[j1]))
				return false;
		}
		
		j1= j0;
		++j0;
	}
	
	return true;
}

// Triangulate a polygon
void glc::triangulate(QList<GLC_Vector2d>& polygon, QList<int>& index, QList<int>& tList)
{
	const int size= polygon.size();	
	if (size == 3)
	{
		tList << index[0] << index[1] << index[2];
		return;
	}
	int i0= 0;
	int i1= 1;
	int i2= 2;
	while(i0 < size)
	{
		if (isDiagonal(polygon, i0, i2))
		{
			// Add the triangle before removing the ear.
			tList << index[i0] << index[i1] << index[i2];
			// Remove the ear from polygon and index lists
			polygon.removeAt(i1);
			index.removeAt(i1);
			// recurse to the new polygon
			triangulate(polygon, index, tList);
			return;
		}
		++i0;
		i1= (i1 + 1) % size;
		i2= (i2 + 1) % size;
	}
}

//! Triangulate polygon wich vertices are in a mesh
QVector<int> glc::triangulateMeshPoly(const GLC_Mesh2* pMesh, const QVector<int>& face)
{
	// Get the mesh's polygon vertexs
	QList<GLC_Vector4d> originVectors;
	int size= face.size();
	for (int i= 0; i < size; ++i)
	{
		originVectors.append(pMesh->getVertex(face[i]));
	}
	
//-------------- Change frame to mach polygon plane	
	// Compute face normal
	const GLC_Vector4d vect1(originVectors[0]);
	const GLC_Vector4d vect2(originVectors[1]);
	const GLC_Vector4d vect3(originVectors[2]);
	
	const GLC_Vector4d edge1(vect2 - vect1);
	const GLC_Vector4d edge2(vect3 - vect2);
	
	GLC_Vector4d polygonPlaneNormal(edge1 ^ edge2);
	polygonPlaneNormal.setNormal(1.0);
	// Find rotation axis between polygon plane and frame plane
	GLC_Vector4d refNormal;
	GLC_Vector4d rotationAxis(polygonPlaneNormal ^ AxeZ);
	if (rotationAxis.isNull())
	{
		rotationAxis= polygonPlaneNormal ^ AxeX;
		if (rotationAxis.isNull())
		{
			rotationAxis= polygonPlaneNormal ^ AxeY;
			refNormal= AxeY;
		}
		else refNormal= AxeX;
	}
	else refNormal= AxeZ;
	
	const double angle= acos(polygonPlaneNormal * refNormal);
	
	// Create the transformation matrix
	GLC_Matrix4x4 transformation(GLC_Matrix4x4(rotationAxis, angle));
	QList<GLC_Vector2d> polygon;
	// Transforme polygon vertexs
	for (int i=0; i < size; ++i)
	{
		qDebug() << "b" << originVectors[i].toString();
		originVectors[i]= transformation * originVectors[i];
		qDebug() << "a" << originVectors[i].toString();
		// Create 2d vector
		polygon << originVectors[i].toVector2d(refNormal);
		qDebug() << polygon[i].toString();
	}
	// Create the index
	QList<int> index;
	for (int i= 0; i < size; ++i)
		index.append(i);

	QList<int> tList;
	triangulate(polygon, index, tList);
	size= tList.size();
	QVector<int> result;
	for (int i= 0; i < size; ++i)
	{
		result.append(face[tList[i]]);
		qDebug() << QString::number(result[i]);
	}
	
	return result;
}







