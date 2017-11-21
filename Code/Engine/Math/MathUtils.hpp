#pragma once

#include <algorithm>
#include <random>
#include <utility>

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Quaternion.hpp"

class AABB2;
class Disc2;
class LineSegment2;
class Plane2;

class AABB3;
class LineSegment3;
class Plane3;
class Sphere3;

namespace MathUtils {

constexpr const float M_PI                         = 3.14159265358979323846f;           // pi
constexpr const float M_E                          = 2.71828182845904523536f;           // e
constexpr const float M_LOG2E                      = 1.44269504088896340736f;           // log2(e)
constexpr const float M_LOG10E                     = 0.43429448190325182765f;           // log10(e)
constexpr const float M_LN2                        = 0.69314718055994530942f;           // ln(2)
constexpr const float M_LN10                       = 2.30258509299404568402f;           // ln(10)
constexpr const float M_PI_2                       = 1.57079632679489661923f;           // pi/2
constexpr const float M_PI_4                       = 0.78539816339744830962f;           // pi/4
constexpr const float M_1_PI                       = 0.31830988618379067151f;           // 1/pi
constexpr const float M_2_PI                       = 0.63661977236758134308f;           // 2/pi
constexpr const float M_2_SQRTPI                   = 1.12837916709551257390f;           // 2/sqrt(pi)
constexpr const float M_SQRT2                      = 1.41421356237309504880f;           // sqrt(2)
constexpr const float M_1_SQRT2                    = 0.70710678118654752440f;           // 1/sqrt(2)
constexpr const float M_SQRT3_3                    = 0.57735026918962576451f;           // sqrt(3)/3
constexpr const float M_TAU                        = 1.61803398874989484821f;           // tau (golden ratio)
constexpr const long double KIB_BYTES_RATIO             = 0.0009765625;                      // Kilobyte/Bytes
constexpr const long double MIB_BYTES_RATIO             = 0.00000095367431640625;            // Megabyte/Bytes
constexpr const long double GIB_BYTES_RATIO             = 0.000000000931322574615478515625;  // Gigabyte/Bytes
constexpr const long double BYTES_KIB_RATIO             = 1024.0;                            // Bytes/Kilobytes
constexpr const long double BYTES_MIB_RATIO             = 1048576.0;                         // Bytes/Megabytes
constexpr const long double BYTES_GIB_RATIO        = 1073741824.0;                      // Bytes/Gigabytes

//NOT THREAD SAFE!
std::random_device& GetRandomDevice();
//NOT THREAD SAFE!
std::mt19937& GetCryptoRandomEngine(unsigned long seed = 0);
//NOT THREAD SAFE!
std::mt19937_64& GetBigCryptoRandomEngine(unsigned long long seed = 0);
//NOT THREAD SAFE!
std::minstd_rand& GetRandomEngine(unsigned long seed = 0);

long double ConvertBytesToMiB(const std::size_t& bytes);
long double ConvertBytesToKiB(const std::size_t& bytes);
long double ConvertBytesToGiB(const std::size_t& bytes);
long double ConvertKiBToBytes(const long double& KiB);
long double ConvertMiBToBytes(const long double& MiB);
long double ConvertGiBToBytes(const long double& GiB);

//Not properly supported in VS2015
//constexpr long double operator "" _kib(unsigned long long bytes);
//constexpr long double operator "" _kib(long double Kib);
//constexpr long double operator "" _mib(unsigned long long bytes);
//constexpr long double operator "" _mib(long double Mib);
//constexpr long double operator "" _gib(unsigned long long bytes);
//constexpr long double operator "" _gib(long double Gib);

//Splits a floating point value into its integer and fractional parts
std::pair<float, float> SplitFloatingPointValue(float value);
std::pair<double, double> SplitFloatingPointValue(double value);
std::pair<long double, long double> SplitFloatingPointValue(long double value);

float ConvertRadiansToDegrees(float radians);
float ConvertDegreesToRadians(float degrees);

float ConvertFahrenheitToCelcius(float F);
float ConvertCelciusToFahrenheit(float C);

int GetRandomIntLessThan(int maxValueNotInclusive);
int GetRandomIntInRange(int minValueInclusive, int maxValueInclusive);

long GetRandomLongLessThan(long maxValueNotInclusive);
long GetRandomLongInRange(long minValueInclusive, long maxValueInclusive);

long long GetRandomLongLongLessThan(long long maxValueNotInclusive);
long long GetRandomLongLongInRange(long long minValueInclusive, long long maxValueInclusive);

float GetRandomFloatZeroToOne();
float GetRandomFloatInRange(float minimumInclusive, float maximumInclusive);
bool IsPercentChance(float probability);

double GetRandomDoubleZeroToOne();
double GetRandomDoubleInRange(double minimumInclusive, double maximumInclusive);
bool IsPercentChance(double probability);

Vector3 GetRandomPointInSphere(const Vector3& pos, float r);
Vector3 GetRandomPointInSphere(const Sphere3& s);

Vector3 GetRandomPointInCube(float r);

float CosDegrees(float degrees);
float SinDegrees(float degrees);
float Atan2Degrees(float y, float x);

void TurnToward(float& currentDegrees, float goalDegrees, float maxTurnDegrees);
float CalcShortestAngularDistance(float startDegrees, float endDegrees);

bool IsEquivalent(float a, float b, float epsilon = 0.0001f);
bool IsEquivalent(double a, double b, double epsilon = 0.0001);
bool IsEquivalent(const Vector2& a, const Vector2& b, float epsilon = 0.0001f);
bool IsEquivalent(const Vector3& a, const Vector3& b, float epsilon = 0.0001f);
bool IsEquivalent(const Vector4& a, const Vector4& b, float epsilon = 0.0001f);
bool IsEquivalent(const Quaternion& a, const Quaternion& b, float epsilon = 0.0001f);

float DotProduct(const Vector2& a, const Vector2& b);
float DotProduct(const Vector3& a, const Vector3& b);
float DotProduct(const Vector4& a, const Vector4& b);
float DotProduct(const Quaternion& a, const Quaternion& b);

Vector2 Project(const Vector2& a, const Vector2& b);
Vector3 Project(const Vector3& a, const Vector3& b);
Vector4 Project(const Vector4& a, const Vector4& b);

Vector2 ProjectAlongPlane(const Vector2& v, const Vector2& n);
Vector3 ProjectAlongPlane(const Vector3& v, const Vector3& n);
Vector4 ProjectAlongPlane(const Vector4& v, const Vector4& n);

bool DoAABBsOverlap(const AABB2& a, const AABB2& b);
bool DoDiscsOverlap(const Disc2& a, const Disc2& b);
bool DoDiscsOverlap(const Vector2& aCenter, float aRadius,
                    const Vector2& bCenter, float bRadius);

bool DoDiscLineSegmentOverlap(const Disc2& a, const LineSegment2& b);
bool DoDiscPlaneOverlap(const Disc2& a, const Plane2& b);

bool DoSpheresOverlap(const Sphere3& a, const Sphere3& b);
bool DoSpheresOverlap(const Vector3& aCenter, float aRadius,
                    const Vector3& bCenter, float bRadius);

bool DoAABB3sOverlap(const AABB3& a, const AABB3& b);
bool DoSphereLineSegmentOverlap(const Sphere3& a, const LineSegment3& b);
bool DoSpherePlaneOverlap(const Sphere3& a, const Plane3& b);
bool IsPointInFrontOfPlane(const Vector3& v, const Plane3& b);
bool IsPointBehindPlane(const Vector3& v, const Plane3& b);

Vector2 ReflectVector(const Vector2& in, const Vector2& normal);
Vector3 ReflectVector(const Vector3& in, const Vector3& normal);
Vector4 ReflectVector(const Vector4& in, const Vector4& normal);

Vector2 Rotate(const Vector2& v, const Quaternion& q);
Vector3 Rotate(const Vector3& v, const Quaternion& q);

unsigned int CalculateManhattanDistance(const IntVector2& start, const IntVector2& end);
unsigned int CalculateManhattanDistance(const IntVector3& start, const IntVector3& end);
unsigned int CalculateManhattanDistance(const IntVector4& start, const IntVector4& end);

namespace EasingFunctions {

template<typename T>
T Square(const T& t) {
    return t * t;
}

template<typename T>
T SmoothStart2(const T& t) {
    return t * t;
}
template<typename T>
T SmoothStart3(const T& t) {
    return t * t * t;
}
template<typename T>
T SmoothStart4(const T& t) {
    return t * t * t * t;
}
template<typename T>
T SmoothStart5(const T& t) {
    return t * t * t * t * t;
}
template<typename T>
T SmoothStart6(const T& t) {
    return t * t * t * t * t * t;
}
template<typename T>
T SmoothStart7(const T& t) {
    return t * t * t * t * t * t * t;
}
template<typename T>
T SmoothStart8(const T& t) {
    return t * t * t * t * t * t * t * t;
}
template<typename T>
T SmoothStart9(const T& t) {
    return t * t * t * t * t * t * t * t * t;
}
template<typename T>
T SmoothStart10(const T& t) {
    return t * t * t * t * t * t * t * t * t * t;
}

template<typename T>
T Flip(const T& t) {
    return 1.0f - t;
}

template<typename T>
T SmoothStop2(const T& t) {
    return 1.0f - (1.0f - t) * (1.0f - t);
}
template<typename T>
T SmoothStop3(const T& t) {
    return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
}
template<typename T>
T SmoothStop4(const T& t) {
    return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);
}
template<typename T>
T SmoothStop5(const T& t) {
    return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);
}
template<typename T>
T SmoothStop6(const T& t) {
    return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);
}
template<typename T>
T SmoothStop7(const T& t) {
    return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);
}
template<typename T>
T SmoothStop8(const T& t) {
    return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);
}
template<typename T>
T SmoothStop9(const T& t) {
    return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);
}
template<typename T>
T SmoothStop10(const T& t) {
    return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);
}

template<typename T>
T SmoothStep2(const T& t) {
    return Interpolate(SmoothStart2(t), SmoothStop2(t), 0.50f);
}
template<typename T>
T SmoothStep3(const T& t) {
    return Interpolate(SmoothStart3(t), SmoothStop3(t), 0.50f);
}
template<typename T>
T SmoothStep4(const T& t) {
    return Interpolate(SmoothStart4(t), SmoothStop4(t), 0.50f);
}
template<typename T>
T SmoothStep5(const T& t) {
    return Interpolate(SmoothStart5(t), SmoothStop5(t), 0.50f);
}
template<typename T>
T SmoothStep6(const T& t) {
    return Interpolate(SmoothStart6(t), SmoothStop6(t), 0.50f);
}
template<typename T>
T SmoothStep7(const T& t) {
    return Interpolate(SmoothStart7(t), SmoothStop7(t), 0.50f);
}
template<typename T>
T SmoothStep8(const T& t) {
    return Interpolate(SmoothStart8(t), SmoothStop8(t), 0.50f);
}
template<typename T>
T SmoothStep9(const T& t) {
    return Interpolate(SmoothStart9(t), SmoothStop9(t), 0.50f);
}
template<typename T>
T SmoothStep10(const T& t) {
    return Interpolate(SmoothStart10(t), SmoothStop10(t), 0.50f);
}
} //End MathUtils::EasingFunctions namespace

template<typename T>
T Clamp(const T& valueToClamp, const T& minRange, const T& maxRange) {
    if(valueToClamp < minRange) {
        return minRange;
    }
    if(maxRange < valueToClamp) {
        return maxRange;
    }
    return valueToClamp;
}

template<>
IntVector2 Clamp<IntVector2>(const IntVector2& valueToClamp, const IntVector2& minRange, const IntVector2& maxRange);

template<>
IntVector3 Clamp<IntVector3>(const IntVector3& valueToClamp, const IntVector3& minRange, const IntVector3& maxRange);

template<>
Vector2 Clamp<Vector2>(const Vector2& valueToClamp, const Vector2& minRange, const Vector2& maxRange);

template<>
Vector3 Clamp<Vector3>(const Vector3& valueToClamp, const Vector3& minRange, const Vector3& maxRange);

template<typename T>
T RangeMap(const T& valueToMap, const T& minInputRange, const T& maxInputRange, const T& minOutputRange, const T& maxOutputRange) {
    return (valueToMap - minInputRange) * (maxOutputRange - minOutputRange) / (maxInputRange - minInputRange) + minOutputRange;
}

template<typename T>
T Interpolate(const T& a, const T& b, float t) {
    return ((1.0f - t) * a) + (t * b);
}

template<>
Rgba Interpolate(const Rgba& a, const Rgba& b, float t);

float CalculateMatrix3Determinant(float m00, float m01, float m02,
                                  float m10, float m11, float m12,
                                  float m20, float m21, float m22);

float CalculateMatrix2Determinant(float m00, float m01,
                                  float m10, float m11);

} //End MathUtils namespace