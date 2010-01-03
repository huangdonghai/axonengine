/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

namespace Axon 
{
Spline::Spline()
	: m_width(0.0f)
	, m_intervalLength(2.0f)
	, m_closed(false)
	, m_dirty(true)
{
	m_controlPoints.clear();
}

Spline::~Spline()
{
	m_controlPoints.clear();
}

Vector3 &Spline::getControlPoint(int pos)
{
	AX_ASSERT(pos < (int) m_controlPoints.size());

	return m_controlPoints[pos];
}

void Spline::addControlPoint(const Vector3 &point)
{
	m_dirty = true;

	m_controlPoints.push_back(point);
}

void Spline::addControlPoint(int pos, const Vector3 &point)
{
	AX_ASSERT(pos <= (int) m_controlPoints.size());

	m_dirty = true;

	m_controlPoints.insert(m_controlPoints.begin()+pos, point);
}

void Spline::deleteControlPoint(int pos)
{
	AX_ASSERT(pos < (int) m_controlPoints.size());

	m_dirty = true;

	m_controlPoints.erase(m_controlPoints.begin()+pos);
}

void Spline::deleteControlPoint(const Vector3 &point)
{
	m_dirty = true;

	std::vector<Vector3>::iterator itr;
	for (itr=m_controlPoints.begin(); itr!=m_controlPoints.end(); ++itr)
	{
		if (*itr == point)
		{
			m_controlPoints.erase(itr);

			break;
		}
	}
}

void Spline::deleteAllControlPoints()
{
	m_dirty = true;

	m_controlPoints.clear();
}

int Spline::getControlPointIndex(const Vector3 &point)
{
	for (int i=0; i<(int)m_controlPoints.size(); ++i)
	{
		if (m_controlPoints[i] == point)
		{
			return i;
		}
	}

	return -1;
}

/*Vector3 &Spline::getControlPoint(const Vector3 &point)
{
	int i = getControlPointIndex(point);

	if (i > 0)
	{
		return m_controlPoints[i];
	}

	return Vector3();
}*/

void Spline::getAllPoints(std::vector<Vector3>& result)
{
	result.clear();

	if (m_controlPoints.size() <= 1)
	{
		return ;
	}
	
	//if (m_dirty == true)
	//{
		generateHelperData();
	//}
	
	generateSplineData(result);

	m_dirty = false;
}

void Spline::clear()
{
	m_width = 0;
	m_intervalLength = 1;
	m_closed = false;

	m_controlPoints.clear();
	m_helpers.clear();
}

void Spline::generateHelperData()
{
	float AMag , AMagOld;
	int numPoint = (int) m_controlPoints.size();

	m_helpers.resize(numPoint);

	// vector A
	for (int i=0; i<=numPoint-2; i++) 
	{
		m_helpers[i].A = m_controlPoints[i+1] - m_controlPoints[i];
	}

	// k
	AMagOld = m_helpers[0].A.getLength();
	for (int i=0; i<=numPoint-3; ++i) 
	{
		AMag = m_helpers[i+1].A.getLength();
		m_helpers[i].k = AMagOld / AMag;
		AMagOld = AMag;
	}

	m_helpers[numPoint-2].k = 1.0f;

	// Matrix
	for (int i=1; i<=numPoint-2; ++i) 
	{
		m_helpers[i].mat[0] = 1.0f;
		m_helpers[i].mat[1] = 2.0f * m_helpers[i-1].k * (1.0f + m_helpers[i-1].k);
		m_helpers[i].mat[2] = m_helpers[i-1].k * m_helpers[i-1].k * m_helpers[i].k;
	}

	m_helpers[0].mat[1] = 2.0f;
	m_helpers[0].mat[2] = m_helpers[0].k;
	m_helpers[numPoint-1].mat[0] = 1.0f;
	m_helpers[numPoint-1].mat[1] = 2.0f * m_helpers[numPoint-2].k;

	// vector B
	for (int i=1; i<=numPoint-2; ++i) 
	{
		m_helpers[i].B.x = 3.0f * (m_helpers[i-1].A.x + m_helpers[i-1].k * m_helpers[i-1].k * m_helpers[i].A.x);
		m_helpers[i].B.y = 3.0f * (m_helpers[i-1].A.y + m_helpers[i-1].k * m_helpers[i-1].k * m_helpers[i].A.y);
		m_helpers[i].B.z = 3.0f * (m_helpers[i-1].A.z + m_helpers[i-1].k * m_helpers[i-1].k * m_helpers[i].A.z);
	}

	m_helpers[0].B = m_helpers[0].A * 3.0f;
	m_helpers[numPoint-1].B = m_helpers[numPoint-2].A * 3.0f;

	matrixSolve();

	// vector C
	for (int i=0; i<=numPoint-2; ++i) 
	{
		m_helpers[i].C = m_helpers[i+1].B * m_helpers[i].k;
	}
}

void Spline::generateSplineData(std::vector<Vector3>& result)
{
	result.clear();

	float  t, f, g, h;
	float len=0;
	int nDiv = 1;
	Vector3 point;

	// 线条模式
	if (m_width == 0)
	{
		for (int i=0; i<(int)m_controlPoints.size()-1; ++i) 
		{
			nDiv = (int)(std::max(std::max(abs(m_helpers[i].A.x), abs(m_helpers[i].A.y)), abs(m_helpers[i].A.z)) / m_intervalLength);
			if (nDiv == 0)
			{
				nDiv = 1;
			}

			point = m_controlPoints[i];
			if (i == 0)
			{
				result.push_back(point);
			}
			else if ((point - result[result.size()-1]).getLength() >= m_intervalLength/5.0f)
			{
				result.push_back(point);
			}
			
			for (int j=1; j<=nDiv; ++j)
			{
				t = 1.0f / (float)nDiv * (float)j;

				AX_ASSERT(t >= 0 && t <= 1.0 && "t is overflow!");

				f = t * t * (3.0f - 2.0f*t);
				g = t * (t-1.0f) * (t-1.0f);
				h = t * t * (t-1.0f);

				point.x = m_controlPoints[i].x + m_helpers[i].A.x * f + m_helpers[i].B.x * g + m_helpers[i].C.x * h;
				point.y = m_controlPoints[i].y + m_helpers[i].A.y * f + m_helpers[i].B.y * g + m_helpers[i].C.y * h;
				point.z = m_controlPoints[i].z + m_helpers[i].A.z * f + m_helpers[i].B.z * g + m_helpers[i].C.z * h;

				// 如果宽度为零, 是线条模式
				result.push_back(point);

			} // for
		} // for
	}
	else // 带状模式
	{
		std::vector<Vector3> tResult;

		for (int i=0; i<(int)m_controlPoints.size()-1; ++i) 
		{
		#if 0
			len = (m_controlPoints[i+1] - m_controlPoints[i]).getLength();
			nDiv = len / m_intervalLength;
		#else
			nDiv = (int)(std::max(std::max(abs(m_helpers[i].A.x), abs(m_helpers[i].A.y)), abs(m_helpers[i].A.z)) / m_intervalLength);
		#endif
			if (nDiv == 0)
			{
				nDiv = 1;
			}

			point = m_controlPoints[i];
			if (i == 0)
			{
				tResult.push_back(point);
			}
			else if ((point - tResult[tResult.size()-1]).getLength() >= m_intervalLength/5.0f)
			{
				tResult.push_back(point);
			}

			for (int j=1; j<=nDiv; ++j)
			{
			#if 0
				t = (float)(m_intervalLength * j) / len;
			#else
				t = 1.0f / (float)nDiv * (float)j;
			#endif
				AX_ASSERT(t >= 0 && t <= 1.0 && "t is overflow!");

				f = t * t * (3.0f - 2.0f*t);
				g = t * (t-1.0f) * (t-1.0f);
				h = t * t * (t-1.0f);

				point.x = m_controlPoints[i].x + m_helpers[i].A.x * f + m_helpers[i].B.x * g + m_helpers[i].C.x * h;
				point.y = m_controlPoints[i].y + m_helpers[i].A.y * f + m_helpers[i].B.y * g + m_helpers[i].C.y * h;
				point.z = m_controlPoints[i].z + m_helpers[i].A.z * f + m_helpers[i].B.z * g + m_helpers[i].C.z * h;

				tResult.push_back(point);
			} // for
		} // for

		int numResult = (int) tResult.size();
		if (numResult < 2)
		{
			return ;
		}

		Vector3 dir = tResult[1] - tResult[0];
		Vector3 crossDir;

		dir.z = 0;
		dir.normalize();
		crossDir = dir ^ Vector3(0, 0, 1);
		crossDir *= m_width / 2.0f;

		result.push_back(tResult[0] + crossDir);
		result.push_back(tResult[0]);
		result.push_back(tResult[0] - crossDir);

		for (int i=1; i<numResult-1; ++i)
		{
			dir = tResult[i+1] - tResult[i-1];
			if (dir.getLength() < 0.001 || (tResult[i+1]-tResult[i]).getLength() < 0.001)
			{
				continue ;
			}

			dir.z = 0;
			dir.normalize();

			Vector3 crossDir = dir ^ Vector3(0, 0, 1);
			crossDir *= m_width / 2.0f;

			result.push_back(tResult[i] + crossDir);
			result.push_back(tResult[i]);
			result.push_back(tResult[i] - crossDir);
		}

		dir = tResult[numResult-1] - tResult[numResult-2];
		dir.z = 0;
		dir.normalize();
		crossDir = dir ^ Vector3(0, 0, 1);
		crossDir *= m_width / 2.0f;

		result.push_back(tResult[numResult-1] + crossDir);
		result.push_back(tResult[numResult-1]);
		result.push_back(tResult[numResult-1] - crossDir);
	}
}

void Spline::matrixSolve() 
{
	int numPoint = (int) m_controlPoints.size();

	float *Work = new float[numPoint];
	float *WorkB = new float[numPoint];

	// ------------- Bx --------------

	for (int i=0; i<=numPoint-1; ++i) 
	{
		Work[i] = m_helpers[i].B.x / m_helpers[i].mat[1];
		WorkB[i] = Work[i];
	}

	for (int j=0; j<10; ++j) 
	{
		//  need convergence judge
		Work[0] = (m_helpers[0].B.x - m_helpers[0].mat[2] * WorkB[1]) / m_helpers[0].mat[1];
		for (int i=1; i<numPoint-1; ++i) 
		{
			Work[i] = (m_helpers[i].B.x - m_helpers[i].mat[0]*WorkB[i-1] - m_helpers[i].mat[2]*WorkB[i+1])
						/ m_helpers[i].mat[1];
		}
		Work[numPoint-1] = (m_helpers[numPoint-1].B.x - m_helpers[numPoint-1].mat[0] * WorkB[numPoint-2])
							/ m_helpers[numPoint-1].mat[1];

		for (int i=0; i<=numPoint-1; ++i) 
		{
			WorkB[i] = Work[i];
		}
	}

	for (int i=0; i<=numPoint-1; ++i) 
	{
		m_helpers[i].B.x = Work[i];
	}

	// ------------- By --------------

	for (int i=0;i<=numPoint-1;i++) 
	{
		Work[i] = m_helpers[i].B.y / m_helpers[i].mat[1];
		WorkB[i] = Work[i];
	}

	for (int j=0; j<10; j++) 
	{
		//  need convergence judge
		Work[0] = (m_helpers[0].B.y - m_helpers[0].mat[2] * WorkB[1]) / m_helpers[0].mat[1];
		for (int i=1; i<numPoint-1 ; i++) 
		{
			Work[i] = (m_helpers[i].B.y - m_helpers[i].mat[0]*WorkB[i-1] - m_helpers[i].mat[2]*WorkB[i+1])
				/ m_helpers[i].mat[1];
		}
		Work[numPoint-1] = (m_helpers[numPoint-1].B.y - m_helpers[numPoint-1].mat[0] * WorkB[numPoint-2])
			/ m_helpers[numPoint-1].mat[1];

		for (int i=0; i<=numPoint-1; ++i) 
		{
			WorkB[i] = Work[i];
		}
	}

	for (int i=0 ; i<=numPoint-1 ; i++) 
	{
		m_helpers[i].B.y = Work[i];
	}

	// ------------- Bz --------------

	for (int i=0;i<=numPoint-1;i++) 
	{
		Work[i] = m_helpers[i].B.z / m_helpers[i].mat[1];
		WorkB[i] = Work[i];
	}

	for (int j=0; j<10; j++) 
	{
		//  need convergence judge
		Work[0] = (m_helpers[0].B.z - m_helpers[0].mat[2] * WorkB[1]) / m_helpers[0].mat[1];
		for (int i=1; i<numPoint-1 ; i++) 
		{
			Work[i] = (m_helpers[i].B.z - m_helpers[i].mat[0]*WorkB[i-1] - m_helpers[i].mat[2]*WorkB[i+1])
				/ m_helpers[i].mat[1];
		}
		Work[numPoint-1] = (m_helpers[numPoint-1].B.z - m_helpers[numPoint-1].mat[0] * WorkB[numPoint-2])
			/ m_helpers[numPoint-1].mat[1];

		for (int i=0; i<=numPoint-1; ++i) 
		{
			WorkB[i] = Work[i];
		}
	}

	for (int i=0 ; i<=numPoint-1 ; i++) 
	{
		m_helpers[i].B.z = Work[i];
	}

	delete[] Work;
	delete[] WorkB;
}

AX_END_NAMESPACE
