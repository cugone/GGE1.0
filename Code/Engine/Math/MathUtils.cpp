#include "Engine/Math/MathUtils.hpp"

#include <cmath>
#include <limits>

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Plane2.hpp"

#include "Engine/Math/Quaternion.hpp"

#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/Sphere3.hpp"

namespace MathUtils {

//NOT THREAD SAFE!
std::random_device& GetRandomDevice() {
    static std::random_device rd;
    return rd;
}

//NOT THREAD SAFE!
std::mt19937& GetCryptoRandomEngine(unsigned long seed /*= 0*/) {
    static std::mt19937 engine = std::mt19937(!seed ? GetRandomDevice()() : seed);
    return engine;
}

//NOT THREAD SAFE!
std::mt19937_64& GetBigCryptoRandomEngine(unsigned long long seed /*= 0*/) {
    static std::mt19937_64 engine = std::mt19937_64(!seed ? GetRandomDevice()() : seed);
    return engine;
}

//NOT THREAD SAFE!
std::minstd_rand& GetRandomEngine(unsigned long seed /*= 0*/) {
    static std::minstd_rand engine = std::minstd_rand(!seed ? GetRandomDevice()() : seed);
    return engine;
}

long double ConvertBytesToKiB(const std::size_t& bytes) {
    return bytes * KIB_BYTES_RATIO;
}

long double ConvertBytesToMiB(const std::size_t& bytes) {
    return bytes * MIB_BYTES_RATIO;
}

long double ConvertBytesToGiB(const std::size_t& bytes) {
    return bytes * GIB_BYTES_RATIO;
}

long double ConvertKiBToBytes(const long double& KiB) {
    return KiB * BYTES_KIB_RATIO;
}

long double ConvertMiBToBytes(const long double& MiB) {
    return MiB * BYTES_MIB_RATIO;
}

long double ConvertGiBToBytes(const long double& GiB) {
    return GiB * BYTES_GIB_RATIO;
}

std::pair<float, float> SplitFloatingPointValue(float value) {
    float frac = 0.0f;
    float int_part = 0.0f;
    frac = std::modf(value, &int_part);
    return std::make_pair(int_part, frac);
}

std::pair<double, double> SplitFloatingPointValue(double value) {
    double frac = 0.0f;
    double int_part = 0.0f;
    frac = std::modf(value, &int_part);
    return std::make_pair(int_part, frac);
}

std::pair<long double, long double> SplitFloatingPointValue(long double value) {
    long double frac = 0.0f;
    long double int_part = 0.0f;
    frac = std::modf(value, &int_part);
    return std::make_pair(int_part, frac);
}

//Not properly supported in VS2015
//constexpr long double operator "" _kib(unsigned long long bytes) {
//    return bytes * KIB_BYTES_RATIO;
//}
//
//constexpr long double operator "" _kib(long double Kib) {
//    return Kib * BYTES_KIB_RATIO;
//}
//
//constexpr long double operator "" _mib(unsigned long long bytes) {
//    return bytes * MIB_BYTES_RATIO;
//}
//
//constexpr long double operator "" _mib(long double Mib) {
//    return Mib * BYTES_MIB_RATIO;
//}
//constexpr long double operator "" _gib(unsigned long long bytes) {
//    return bytes * GIB_BYTES_RATIO;
//}
//
//constexpr long double operator "" _gib(long double Gib) {
//    return Gib * BYTES_GIB_RATIO;
//}


template<>
IntVector2 Clamp<IntVector2>(const IntVector2& valueToClamp, const IntVector2& minRange, const IntVector2& maxRange) {
    IntVector2 result = valueToClamp;
    result.x = Clamp(valueToClamp.x, minRange.x, maxRange.x);
    result.y = Clamp(valueToClamp.y, minRange.y, maxRange.y);
    return result;
}

template<>
IntVector3 Clamp<IntVector3>(const IntVector3& valueToClamp, const IntVector3& minRange, const IntVector3& maxRange) {
    IntVector3 result = valueToClamp;
    result.x = Clamp(valueToClamp.x, minRange.x, maxRange.x);
    result.y = Clamp(valueToClamp.y, minRange.y, maxRange.y);
    result.z = Clamp(valueToClamp.z, minRange.z, maxRange.z);
    return result;
}

template<>
Vector2 Clamp<Vector2>(const Vector2& valueToClamp, const Vector2& minRange, const Vector2& maxRange) {
    Vector2 result = valueToClamp;
    result.x = Clamp(valueToClamp.x, minRange.x, maxRange.x);
    result.y = Clamp(valueToClamp.y, minRange.y, maxRange.y);
    return result;
}

template<>
Vector3 Clamp<Vector3>(const Vector3& valueToClamp, const Vector3& minRange, const Vector3& maxRange) {
    Vector3 result = valueToClamp;
    result.x = Clamp(valueToClamp.x, minRange.x, maxRange.x);
    result.y = Clamp(valueToClamp.y, minRange.y, maxRange.y);
    result.z = Clamp(valueToClamp.z, minRange.z, maxRange.z);
    return result;
}

template<>
Rgba Interpolate(const Rgba& a, const Rgba& b, float t) {
    return Interpolate(a, b, t);
}

float CalculateMatrix3Determinant(float m00, float m01, float m02,
                                  float m10, float m11, float m12,
                                  float m20, float m21, float m22) {
    float a = m00;
    float b = m01;
    float c = m02;
    float det_not_a = CalculateMatrix2Determinant(m11, m12, m21, m22);
    float det_not_b = CalculateMatrix2Determinant(m10, m12, m20, m22);
    float det_not_c = CalculateMatrix2Determinant(m10, m11, m20, m21);

    return a * det_not_a - b * det_not_b + c * det_not_c;
}

float CalculateMatrix2Determinant(float m00, float m01,
                                  float m10, float m11) {
    return m00 * m11 - m01 * m10;
}

float ConvertRadiansToDegrees(float radians) {
	return radians * 180.0f * M_1_PI;
}

float ConvertDegreesToRadians(float degrees) {
	return degrees * (M_PI / 180.0f);
}

float ConvertFahrenheitToCelcius(float F) {
    return (F - 32.0f) * (5.0f / 9.0f);
}

float ConvertCelciusToFahrenheit(float C) {
    return C * (9.0f / 5.0f) + 32.0f;
}

Vector3 GetRandomPointInSphere(const Sphere3& s) {
    float r = s.radius;
    Vector3 p = GetRandomPointInCube(r);
    while(p.CalcLength() > r) {
        p = GetRandomPointInCube(r);
    }
    return p + s.center;
}

Vector3 GetRandomPointInSphere(const Vector3& pos, float r) {
    return GetRandomPointInSphere(Sphere3(pos, r));
}
Vector3 GetRandomPointInCube(float r) {
    float x = GetRandomFloatInRange(-1.0f, 1.0f) * r;
    float y = GetRandomFloatInRange(-1.0f, 1.0f) * r;
    float z = GetRandomFloatInRange(-1.0f, 1.0f) * r;
    return Vector3(x, y, z);
}
float CosDegrees(float degrees) {
	float radians = ConvertDegreesToRadians(degrees);
	return std::cosf(radians);
}

float SinDegrees(float degrees) {
	float radians = ConvertDegreesToRadians(degrees);
	return std::sinf(radians);
}

float Atan2Degrees(float y, float x) {
	float radians = std::atan2f(y, x);
	return ConvertRadiansToDegrees(radians);
}

void TurnToward(float& currentDegrees, float goalDegrees, float maxTurnDegrees) {
    float signedAngularDistance = CalcShortestAngularDistance(currentDegrees, goalDegrees);
    if(std::fabs(signedAngularDistance) <= maxTurnDegrees) {
        currentDegrees = goalDegrees;
    } else if(signedAngularDistance > 0.0f) {
        currentDegrees += maxTurnDegrees;
    } else {
        currentDegrees -= maxTurnDegrees;
    }
}

float CalcShortestAngularDistance(float startDegrees, float endDegrees) {
    float signedAngularDistance = endDegrees - startDegrees;
    while(signedAngularDistance > 180.0f) {
        signedAngularDistance -= 360.0f;
    }
    while(signedAngularDistance < -180.0f) {
        signedAngularDistance += 360.0f;
    }
    return signedAngularDistance;
}

int GetRandomIntLessThan(int maxValueNotInclusive) {
    int randomInt = std::rand();
	return randomInt % maxValueNotInclusive;
}

int GetRandomIntInRange(int minValueInclusive, int maxValueInclusive) {
    int numPossibilities = (maxValueInclusive - minValueInclusive + 1);
    return minValueInclusive + GetRandomIntLessThan(numPossibilities);
}

long GetRandomLongLessThan(long maxValueNotInclusive) {
    std::uniform_int_distribution<long> dist(0, maxValueNotInclusive - 1);
    return dist(GetRandomEngine());
}

long GetRandomLongInRange(long minValueInclusive, long maxValueInclusive) {
    std::uniform_int_distribution<long> dist(minValueInclusive, maxValueInclusive);
    return dist(GetRandomEngine());
}

long long GetRandomLongLongLessThan(long long maxValueNotInclusive) {
    std::uniform_int_distribution<long long> dist(0, maxValueNotInclusive - 1);
    return dist(GetRandomEngine());
}

long long GetRandomLongLongInRange(long long minValueInclusive, long long maxValueInclusive) {
    std::uniform_int_distribution<long long> dist(minValueInclusive, maxValueInclusive);
    return dist(GetRandomEngine());
}

float GetRandomFloatZeroToOne() {
	return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

float GetRandomFloatInRange(float minimumInclusive, float maximumInclusive) {
    return minimumInclusive + (GetRandomFloatZeroToOne() * (maximumInclusive - minimumInclusive));
}

bool IsPercentChance(float probability) {
    float roll = GetRandomFloatZeroToOne();
    return (roll < probability);
}

double GetRandomDoubleZeroToOne() {
    std::uniform_real_distribution<double> dist; //defaults to [0.0,1.0)
    return dist(GetRandomEngine());
}

double GetRandomDoubleInRange(double minimumInclusive, double maximumInclusive) {
    return minimumInclusive + (GetRandomDoubleZeroToOne() * (maximumInclusive - minimumInclusive));
}

bool IsPercentChance(double probability) {
    double roll = GetRandomDoubleZeroToOne();
    return (roll < probability);
}

long double GetRandomLongDoubleZeroToOne() {
    std::uniform_real_distribution<long double> dist; //defaults to [0.0,1.0)
    return dist(GetRandomEngine());
}

long double GetRandomLongDoubleInRange(long double minimumInclusive, long double maximumInclusive) {
    return minimumInclusive + (GetRandomLongDoubleZeroToOne() * (maximumInclusive - minimumInclusive));
}

bool IsPercentChance(long double probability) {
    long double roll = GetRandomLongDoubleZeroToOne();
    return (roll < probability);
}

bool IsEquivalent(float a, float b, float epsilon /*= 0.0001f*/) {
    return std::abs(a - b) < epsilon;
}

bool IsEquivalent(double a, double b, double epsilon /*= 0.0001*/) {
    return std::abs(a - b) < epsilon;
}

bool IsEquivalent(const Vector2& a, const Vector2& b, float epsilon /*= 0.0001f*/) {
    return IsEquivalent(a.x, b.x, epsilon) && IsEquivalent(a.y, b.y, epsilon);
}

bool IsEquivalent(const Vector3& a, const Vector3& b, float epsilon /*= 0.0001f*/) {
    return IsEquivalent(a.x, b.x, epsilon) && IsEquivalent(a.y, b.y, epsilon) && IsEquivalent(a.z, b.z, epsilon);
}

bool IsEquivalent(const Vector4& a, const Vector4& b, float epsilon /*= 0.0001f*/) {
    return IsEquivalent(a.x, b.x, epsilon) && IsEquivalent(a.y, b.y, epsilon) && IsEquivalent(a.z, b.z, epsilon) && IsEquivalent(a.w, b.w, epsilon);
}

bool IsEquivalent(const Quaternion& a, const Quaternion& b, float epsilon /*= 0.0001f*/) {
    return IsEquivalent(a.w, b.w, epsilon) && IsEquivalent(a.axis, b.axis, epsilon);
}

float DotProduct(const Vector2& a, const Vector2& b) {
    return a.x * b.x + a.y * b.y;
}

float DotProduct(const Vector3& a, const Vector3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float DotProduct(const Vector4& a, const Vector4& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float DotProduct(const Quaternion& a, const Quaternion& b) {
    return (a.w * b.w) + DotProduct(a.axis, b.axis);
}

Vector2 Project(const Vector2& a, const Vector2& b) {
    return (DotProduct(a, b) / DotProduct(b, b)) * b;
}

Vector3 Project(const Vector3& a, const Vector3& b) {
    return (DotProduct(a, b) / DotProduct(b, b)) * b;
}

Vector4 Project(const Vector4& a, const Vector4& b) {
    return (DotProduct(a, b) / DotProduct(b, b)) * b;
}

Vector2 ProjectAlongPlane(const Vector2& v, const Vector2& n) {
    return v - (DotProduct(v, n) * n);
}

Vector3 ProjectAlongPlane(const Vector3& v, const Vector3& n) {
    return v - (DotProduct(v, n) * n);
}

Vector4 ProjectAlongPlane(const Vector4& v, const Vector4& n) {
    return v - (DotProduct(v, n) * n);
}
bool DoAABBsOverlap(const AABB2& a, const AABB2& b) {

    if(a.maxs.x < b.mins.x) {
        return false;
    }
    if(b.maxs.x < a.mins.x) {
        return false;
    }
    if(a.maxs.y < b.mins.y) {
        return false;
    }
    if(b.maxs.y < a.mins.y) {
        return false;
    }

    return true;
}

bool DoDiscsOverlap(const Disc2& a, const Disc2& b) {
    return DoDiscsOverlap(a.center, a.radius, b.center, b.radius);
}

bool DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius) {
    return CalcDistanceSquared(aCenter, bCenter) < (aRadius + bRadius) * (aRadius + bRadius);
}

bool DoDiscLineSegmentOverlap(const Disc2& a, const LineSegment2& b) {
    return CalcDistanceSquared(a.center, b) < a.radius * a.radius;
}

bool DoSphereLineSegmentOverlap(const Sphere3& a, const LineSegment3& b) {
    return CalcDistanceSquared(a.center, b) < a.radius * a.radius;
}

bool DoDiscPlaneOverlap(const Disc2& a, const Plane2& b) {
    return std::fabs(DotProduct(a.center, b.normal) - b.dist) < a.radius;
}

bool DoSpheresOverlap(const Sphere3& a, const Sphere3& b) {
    return DoSpheresOverlap(a.center, a.radius, b.center, b.radius);
}

bool DoSpheresOverlap(const Vector3& aCenter, float aRadius, const Vector3& bCenter, float bRadius) {
    return CalcDistanceSquared(aCenter, bCenter) < (aRadius + bRadius) * (aRadius + bRadius);
}

bool DoAABB3sOverlap(const AABB3& a, const AABB3& b) {

    if(a.maxs.x < b.mins.x) {
        return false;
    }
    if(b.maxs.x < a.mins.x) {
        return false;
    }
    if(a.maxs.y < b.mins.y) {
        return false;
    }
    if(b.maxs.y < a.mins.y) {
        return false;
    }
    if(a.maxs.z < b.mins.z) {
        return false;
    }
    if(b.maxs.z < a.maxs.z) {
        return false;
    }
    return true;
}

bool DoSpherePlaneOverlap(const Sphere3& a, const Plane3& b) {
    return std::fabs(DotProduct(a.center, b.normal) - b.dist) < a.radius;
}

bool IsPointInFrontOfPlane(const Vector3& point, const Plane3& plane) {
    return (DotProduct(point, plane.normal) > plane.dist);
}
bool IsPointBehindPlane(const Vector3& point, const Plane3& plane) {
    return (DotProduct(point, plane.normal) < plane.dist);
}

Vector2 ReflectVector(const Vector2& in, const Vector2& normal) {
    return in - ((2.0f * DotProduct(in, normal)) * normal);
}

Vector3 ReflectVector(const Vector3& in, const Vector3& normal) {
    return in - ((2.0f * DotProduct(in, normal)) * normal);
}

Vector4 ReflectVector(const Vector4& in, const Vector4& normal) {
    return in - ((2.0f * DotProduct(in, normal)) * normal);
}

Vector2 Rotate(const Vector2& v, const Quaternion& q) {
    return Rotate(Vector3(v, 0.0f), q);
}

Vector3 Rotate(const Vector3& v, const Quaternion& q) {
    return (q * v * q.CalcInverse()).axis;
}

unsigned int CalculateManhattanDistance(const IntVector2& start, const IntVector2& end) {
    return std::abs(end.x - start.x) + std::abs(end.y - start.y);
}
unsigned int CalculateManhattanDistance(const IntVector3& start, const IntVector3& end) {
    return std::abs(end.x - start.x) + std::abs(end.y - start.y) + std::abs(end.z - start.z);
}
unsigned int CalculateManhattanDistance(const IntVector4& start, const IntVector4& end) {
    return std::abs(end.x - start.x) + std::abs(end.y - start.y) + std::abs(end.z - start.z) + std::abs(end.w - start.w);
}

} //End MathUtils