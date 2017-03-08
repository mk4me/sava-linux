#pragma once
#ifndef PartitionMath_h__
#define PartitionMath_h__

namespace clustering
{	
	namespace partition
	{
		class Math
		{
		public:
			union Vector
			{
				Vector() { }
				Vector(int x, int y, int z) : x(x), y(y), z(z) { }
		
				struct
				{
					int x;
					int y;
					int z;
				};
				int v[3];
			};
		
			static void initialize();
		
			static float length(const Vector& v);
		
			static Vector cross(const Vector& v1, const Vector& v2);
		
			static float distance(const Vector& line1, const Vector& line2, const Vector& point);
		
			static float getAngle(float angle1, float angle2);
			static inline short getAngle(short angle1, short angle2) { return s_Angles[angle1][angle2]; }
		
		private:
			static short s_Angles[361][361];
		};
	}
}

#endif // PartitionMath_h__
