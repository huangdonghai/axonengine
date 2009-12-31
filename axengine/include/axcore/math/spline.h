#ifndef __AX_CORE_Spline_H__
#define __AX_CORE_Spline_H__


namespace Axon 
{

#if 0

	// 曲线
	class Curve
	{
	public:
		/*float  A.x,A.y;
		float  B.x,B.y;
		float  C.x,C.y;*/

		Vector3 A;
		Vector3 B;
		Vector3 C;

		int    nDiv;

		Curve(const Vector3 &A_, const Vector3 &B_, const Vector3 &C_, int m_nDiv) 
			: A(A_)
			, B(B_)
			, C(C_)
			, nDiv(m_nDiv)
		{

		}

		Curve(const Vector3 &A_, const Vector3 &B_, const Vector3 &C_) 
			: A(A_)
			, B(B_)
			, C(C_)
		{
			nDiv = (int)(std::max(abs((int)A.x),abs((int)A.y))/DIV_FACTOR);
		}

		Curve() 
		{	
		};

		void putCurve(const Vector3 &A_, const Vector3 &B_, const Vector3 &C_) 
		{
			A = A_;
			B = B_;
			C = C_;

			nDiv = (int)(std::max(abs((int)A.x),abs((int)A.y))/DIV_FACTOR);
		}

		void GetCurve(float x,float y, Point points[], int &PointCount)
		{
			int X,Y;
			float  t,f,g,h;
			if (nDiv==0)
				nDiv=1;

			X = (int)x; 
			Y= (int)y;
			points[PointCount].x = X;
			points[PointCount].y = Y;
			PointCount++;

			for (int i=1; i<=nDiv ; i++)
			{
				t = 1.0f / (float)nDiv * (float)i;
				f = t*t*(3.0f-2.0f*t);
				g = t*(t-1.0f)*(t-1.0f);
				h = t*t*(t-1.0f);
				X = (int)(x + A.x*f + B.x*g + C.x*h);
				Y = (int)(y + A.y*f + B.y*g + C.y*h);
				points[PointCount].x = X;
				points[PointCount].y = Y;
				PointCount++;
			}
		}
	};

#endif


	// 样条
	class AX_API Spline
	{
	public:
		Spline();
		virtual ~Spline();

		// 计算过程的辅助物
		struct Helper 
		{
			// 系数
			Vector3 A;
			Vector3 B;
			Vector3 C;
			
			float k;
			float mat[3];
		};
		

		void setWidth(float width) { m_width = width; };
		float getWidth() { return m_width; }
		void setIntervalLength(float len) { m_intervalLength = len; }
		float getIntervalLength() { return m_intervalLength; }
		void setClosed(bool closed)	{ m_closed = closed; };
		bool isClosed() { return m_closed; }
		void setDirty(bool dirty) { m_dirty = dirty; }
		bool isDirty() { return m_dirty; }

		int getNumControlPoint() { return (int) m_controlPoints.size(); }
		Vector3 &getControlPoint(int pos);
		//Vector3 &getControlPoint(const Vector3 &point);
		void addControlPoint(const Vector3 &point);
		void addControlPoint(int pos, const Vector3 &point);
		void deleteControlPoint(int pos);
		void deleteControlPoint(const Vector3 &point);
		void deleteAllControlPoints();

		int getControlPointIndex(const Vector3 &point);

		// 根据控制点集获取所有的曲线.若将宽度设为零,返回线段集合.
		void getAllPoints(std::vector<Vector3>& result);

		void clear();

		std::vector<Vector3>& getControlPointArray() { return m_controlPoints;}

	protected:
		void generateSplineData(std::vector<Vector3>& result);

		void generateHelperData();
		void matrixSolve();

		// closed
		void generateClosedData();
		void matrixClosedSolve();

		float m_width;			// 样条曲线宽度
		float m_intervalLength;// 样条采样间隔
		std::vector<Vector3>	m_controlPoints;	// 控制点集
		bool m_closed;		// 是否循环(即首尾相接)
		std::vector<Helper>		m_helpers;		// 计算的辅助物
		bool m_dirty;
	};
	
}

#endif