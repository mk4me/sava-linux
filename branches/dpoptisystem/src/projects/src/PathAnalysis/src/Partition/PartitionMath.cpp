#include "PartitionMath.h"

#include <math.h>
#include <cmath>

namespace clustering
{	
	namespace partition
	{
		short Math::s_Angles[361][361];
		
		void Math::initialize()
		{
			for (int i = 0; i <= 360; ++i)
			{
				for (int j = 0; j <= 360; ++j)
					s_Angles[i][j] = (short)getAngle((float)(i - 180), (float)(j - 180));
			}
		}
		
		float Math::length(const Vector& v)
		{
			return sqrt(float(v.x * v.x + v.y * v.y + v.z * v.z));
		}
		
		Math::Vector Math::cross(const Vector& v1, const Vector& v2)
		{
			Vector ret;
			ret.x = v1.y * v2.z - v1.z * v2.y;
			ret.y = v1.z * v2.x - v1.x * v2.z;
			ret.z = v1.x * v2.y - v1.y * v2.x;
		
			return ret;
		}
		
		float Math::distance(const Vector& line1, const Vector& line2, const Vector& point)
		{
			Vector u(line2.x - line1.x, line2.y - line1.y, line2.z - line1.z);
			Vector pp0(line1.x - point.x, line1.y - point.y, line1.z - point.z);
		
			return length(cross(pp0, u)) / length(u);
		}
		
		float Math::getAngle(float angle1, float angle2)
		{
			if (angle1 * angle2 >= 0.0f)
				return std::abs(angle2 - angle1);
		
			if (angle1 < 0.0f)
				angle1 += 360.0f;
			else
				angle2 += 360.0f;
			return std::abs(angle2 - angle1);
		}
	}
}
