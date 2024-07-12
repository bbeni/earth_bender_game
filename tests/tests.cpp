#include "pch.h"
#include "CppUnitTest.h"
#include "../src/mathematics.hpp"
#include "../src/mathematics.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
	TEST_CLASS(test_mathematics)
	{
	public:
		
		TEST_METHOD(test_mat4_multiply)
		{
			// example of https://computergraphics.stackexchange.com/questions/8449/need-help-with-change-of-basis-world-to-camera-frame
			Mat4 R_inv = { 0 };

			R_inv.u22 = 1.0f / sqrtf(2);
			R_inv.u12 = -1.0f / sqrtf(2);
			R_inv.u31 =	1.0f / sqrtf(2);
			R_inv.u22 = 1.0f / sqrtf(2);

			R_inv.u44 = 1.0f;
			
			Mat4 T_inv = matrix_unit();
			T_inv.u41 = -3.0f;
			T_inv.u42 = -3.0f;

			//@Incomplete

		}

		TEST_METHOD(test_cross1)
		{
			Vec3 x = { 1.0f, 0.0f, 0.0f };
			Vec3 y = { 0.0f, 1.0f, 0.0f };
			Vec3 z = cross(x, y);
			Assert::AreEqual(z.x, 0.0f);
			Assert::AreEqual(z.y, 0.0f);
			Assert::AreEqual(z.z, 1.0f);
		}

		TEST_METHOD(test_cross2)
		{
			Vec3 x = { 0.0f, 0.0f, 1.0f };
			Vec3 y = { 0.0f, -1.0f, 0.0f };
			Vec3 z = cross(x, y);
			Assert::AreEqual(z.x, 1.0f);
			Assert::AreEqual(z.y, 0.0f);
			Assert::AreEqual(z.z, 0.0f);
		}

		TEST_METHOD(test_cross3)
		{
			Vec3 x = { 1.0f, 1.0f, 0.0f };
			Vec3 y = { 0.0f, 1.0f, 1.0f };
			Vec3 z = cross(x, y);
			Assert::AreEqual(z.x, 1.0f);
			Assert::AreEqual(z.y, -1.0f);
			Assert::AreEqual(z.z, 1.0f);
		}

		TEST_METHOD(test_cross4)
		{
			Vec3 x = { 0.0f, 100.0f, 0.0f };
			Vec3 y = { 0.0f, 1221.0f, 0.0f };
			Vec3 z = cross(x, y);
			Assert::AreEqual(z.x, 0.0f);
			Assert::AreEqual(z.y, 0.0f);
			Assert::AreEqual(z.z, 0.0f);
		}

		TEST_METHOD(test_dot)
		{
			Vec3 x = { 1.0f, 2.0f, 3.0f };
			Vec3 y = { -4.0f, -5.0f, -6.0f };
			Assert::IsTrue(dot(x, y) == -32.0f);
		}

		TEST_METHOD(test_normalize_or_z_axis)
		{

			Vec3 x = { 0.0f, 0.0f, 0.0f };
			normalize_or_z_axis(&x);
			Assert::IsTrue(x.x == 0.0f && x.y == 0.0f && x.z == 1.0f);

			//Vec3 x = { 3.0f, 4.0f, 12.0f };
			//normalize_or_z_axis(&x);
			//Assert::IsTrue(x.x == 3.0f / 13 && x.y == 4.0f / 13 && x.z == 12.0f / 13);
		}

	};
}

//float dot(const Vec3& a, const Vec3& b);
//Vec3 cross(const Vec3& a, const Vec3& b);
//void normalize_or_z_axis(Vec3* v);
