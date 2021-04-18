#include "Geometry.h"


bool Plane::Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const
{
	surf.t = dot((point - ray.o), normal) / dot(ray.d, normal);
	// точка нахождения луча в момент времени - точка испускания луча / направление движения луча на нормаль = отрезок времени
	
	if (surf.t > t_min && surf.t < t_max)
	{
		surf.hit = true;// есть пересечение
		surf.hitPoint = ray.o + surf.t * ray.d; // точка пересечения
		//высчитываем по формуле луча
		surf.normal = normal;// вектор нормали к поверхности в точке пересечения
		surf.m_ptr = m_ptr;//материал объекта, с которым нашли пересечение
		return true;
	}

	return false;
}
//Для пересечения со сферой подставляем луч в уравнение сферы и решаем получившееся квадратное уравнение
//Решения этого уравнения - точки пересечения луча со сферой
bool Sphere::Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const
{

	float3 k = ray.o - center;   
	// расчет коэффициентов для уравнения
	float a = dot(ray.d, ray.d); // берем квадрат вектора скорости
	float b = dot(2 * k, ray.d);// 2 * (точка испускания луча - центр сферы) * вектор скорости
	float c = dot(k, k) - r_sq; // разность точки испускания и центра - квадрат радиуса

	float d = b * b - 4 * a * c;

	if (d < 0) return false; //пересечения нет

	surf.t = (-b - sqrt(d)) / 2 * a;//первый корень
	if (surf.t > t_min && surf.t < t_max) //проверка границ и присвоение модифицированных значений
	{
		surf.hit = true;
		surf.hitPoint = ray.o + surf.t * ray.d;
		// расчет точки пересечения по уравнению вектора (луча)
		surf.normal = normalize(surf.hitPoint - center);
		// мы пересчитываем нормаль для новой поверхности 
		surf.m_ptr = m_ptr;
		return true;
	}
	 
	surf.t = (-b + sqrt(d)) / 2 * a;//второй корень
	if (surf.t > t_min && surf.t < t_max) //проверка границ и присвоение модифицированных значений
	{
		surf.hit = true;
		surf.hitPoint = ray.o + surf.t * ray.d;
		// расчет точки пересечения по уравнению вектора (луча)
		surf.normal = normalize(surf.hitPoint - center);
		// мы пересчитываем нормаль для новой поверхности 
		surf.m_ptr = m_ptr;
		return true;
	}

	return false;
}

// Выровнен по осям координат поэтому строим по 2 точкам
bool Parallel::Intersect(const Ray& ray, float tmin, float tmax, SurfHit& surf) const
{
	// Противоположные грани рассматриваемого прямоугольного параллелепипеда лежат в плоскостях, 
   // паралелльных координатным плоскостям
   // находим для каждой координаты t (координаты вершин параллелепипеда - координаты источника луча / координаты вектора времени)

	float x1 = (t_min.x - ray.o.x) * (1.0f / ray.d.x);
	float x2 = (t_max.x - ray.o.x) * (1.0f / ray.d.x);
	float y1 = (t_min.y - ray.o.y) * (1.0f / ray.d.y);
	float y2 = (t_max.y - ray.o.y) * (1.0f / ray.d.y);
	float z1 = (t_min.z - ray.o.z) * (1.0f / ray.d.z);
	float z2 = (t_max.z - ray.o.z) * (1.0f / ray.d.z);
	// Если оба параметра отрицательны - значит, луч не пересекает эту пару плоскостей, т.е. не пересекает и параллелепипед.
	// берем максимальное значение по осям для ближнего расстояния(tmin) и минимальное для дальнего(tmax).

	float tMin = max(max(min(x1, x2), min(y1, y2)), min(z1, z2));
	float tMax = min(min(max(x1, x2), max(y1, y2)), max(z1, z2));

	// Пересечение с кубоидом существует, если tmin <= tmax и tmax > 0
	surf.t = tMin;
	if (tMin < tMax && tMax > 0 && surf.t > tmin && surf.t < tmax) { //проверка границ и присвоение модифицированных значений 
		surf.hit = true;
		surf.hitPoint = ray.o + surf.t * ray.d;
		surf.normal = normalize(surf.hitPoint);
		surf.m_ptr = m_ptr;
		return true;
	}
	return false;
}

bool Triangle::Intersect(const Ray& ray, float tmin, float tmax, SurfHit& surf) const
{
	float3 E1 = b - a; // вектор одной стороны
	float3 E2 = c - a; // вектор другой стороны
	float3 T = ray.o - a; // a - точка, которую мы перенесли в новый центр координат (барицентрические координаты)
	float3 D = ray.d;// вектор скорости
	float3 P = cross(D, E2);
	float3 Q = cross(T, E1);
	float det = dot(E1, P); // время

	if (det < tmin && det > tmax) {
		return false;
	}

	float invDet = 1 / det;
	// барицентрические координаты u и v должны удовлетворять условиям (оба больше 0, но сумма меньше 1)
  // расчитываем три параметра из матрицы
	float u = dot(T, P) * invDet;
	if (u < 0 || u > 1) {
		return false;
	}
	float v = dot(ray.d, Q) * invDet;
	if (v < 0 || u + v > 1) {
		return false;
	}

	surf.t = dot(E2, Q) * invDet;
	if (surf.t > tmin && surf.t < tmax) {//проверка границ и присвоение модифицированных значений 
		surf.hit = true;
		surf.hitPoint = float3(surf.t, u, v);  // находим точку пересечения
		surf.normal = cross((b - a), (c - a)); // находим нормаль через перемножение векторов плоскости
		surf.m_ptr = m_ptr;
		return true;
	}
	return false;
}

bool Square::Intersect(const Ray& ray, float tmin, float tmax, SurfHit& surf) const
{

	float3 d = float3(a.x + c.x - b.x, a.y + c.y - b.y, a.z + a.z - b.z); // четвертая точка квадрата


   // задается через два треугольника, в которые непосредственно и передаем нужный нам цвет


	if (Triangle(a, b, c, new IdealMirror(float3(0.0f, 1.0f, 127 / float(255)))).Intersect(ray, tmin, tmax, surf))
		return true;

	if (Triangle(a, d, c, new IdealMirror(float3(0.0f, 1.0f, 127 / float(255)))).Intersect(ray, tmin, tmax, surf))
		return true;


	return false;

	/*
	//return false;
	float3 p1;//вершина квадрата (одна из)
	float3 n;//нормаль к квадрату
	float3 e1;//ширина НО вектором
	float3 e2;//высота (аналогично)
	p1 = a;
	e1 = b - a;
	e2 = c - a;
	n = cross(e1, e2);
	float t;
	t = dot((p1 - ray.o), n) / dot(ray.d, n);
	if (t >= 0)
	{
		float3 intersect_point;
		intersect_point = (ray.o + ray.d);
		float3 v;
		v = intersect_point - p1;
		float width, height;
		width = length(e1);
		height = length(e2);
		float proj1, proj2;
		proj1 = dot(v, e1) / width;
		proj2 = dot(v, e2) / height;
		if ((proj1 < width && proj1>0) && (proj2 < height && proj2>0))
		{
			surf.hit = true;
			surf.hitPoint = intersect_point;
			surf.m_ptr = m_ptr;
			surf.t = length(ray.o - intersect_point);
			surf.normal = n;

			return true;
		}
		else return false;
	}
	else { return false; }*/
}
