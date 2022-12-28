//
// Library to store polygons
// Created by malva on 01-05-21.
//

#ifndef ELEM_GEOMETRICOS_POLIGONO_H
#define ELEM_GEOMETRICOS_POLIGONO_H

#include "elem_geometricos.h"
#include "Segmento.h"
#include <math.h>
#include <initializer_list>

/*
 * Class for storing a polygon. The polygon's vertices are stored in the array
 * m_puntos and the edges are symbolically given by connecting the vertices in
 * order (vi with v(i+1) until the last vertex) and closing the figure with the
 * edge from the last point to v0.
 */
template <class T>
class Poligono
{
private:
    int m_length{};
    Punto<T>* m_puntos{};

public:
    /*
     * Creates a polygon given a list of points Punto of the same type as the
     * Poligono.
     */
    Poligono(std::initializer_list<Punto<T>> puntos)
    {
        int length = static_cast<int>(puntos.size());
        m_length = length;
        m_puntos = new Punto<T>[static_cast<unsigned long>(length)]{};

        Punto<T>* ppunto{m_puntos};
        for(Punto<T> punto: puntos)
        {
            *ppunto = punto;
            ++ppunto;
        }
    };

    /*
     * Disallow copies of polygons
     */
    Poligono<T>(const Poligono<T>&) = delete;

    /*
     * Disallow Poligono assignment
     */
    Poligono& operator=(const Poligono<T>&) = delete;

    /*
     * Gets the point from the vertex list at the position given by index.
     * This Punto can be edited.
     */
    Punto<T>& operator[] (int index);

    /*
     * Gets the point from the vertex list at the position given by index.
     * This Punto may not be edited.
     */
    const Punto<T>& operator[] (int index) const;

    /*
     * Defines the destructor for Poligono. Deallocates the memory of the
     * vertex array.
     */
    ~Poligono();

    /*
     * Returns the amount of vertices this polygon has.
     */
    int getLength() const { return m_length; }

    /*
     * Returns the value of the signed angle formed with the vertices from the
     * positions previous to centerIndex, centerIndex itself, and the one next
     * to it.
     */
    T signedAngle(int centerIndex) const;

    /*
     * Returns double the signed area of the whole polygon.
     */
    T doubleSignedArea() const;

    /*
     * Calculates the area of the polygon. Returned value is positive.
     */
    double area() const;

    /*
     * Checks whether or not the vertices of the polygon are given in counter
     * clockwise order.
     * The method used is based on the sign of the area.
     */
    bool isCCW() const;

    /*
     * Checks if a Punto p lies inside this Poligono using the odd-even
     * algorithm
     */
    bool pointInside(const Punto<T> &p) const;

    template <class S>
    friend std::ostream& operator<< (std::ostream &out, const Poligono<S> &pol);


};

template<class T>
Punto<T> &Poligono<T>::operator[](int index) {
    return m_puntos[index];
}

template<class T>
const Punto<T> &Poligono<T>::operator[](int index) const {
    return m_puntos[index];
}

template<class T>
Poligono<T>::~Poligono()
{
    delete[] m_puntos;
}

template<class T>
T Poligono<T>::signedAngle(int centerIndex) const {
    int prevIndex{ ((centerIndex-1)%m_length + m_length)%m_length };
    int nextIndex{ (centerIndex+1)%m_length};

    Punto<T> centerP{m_puntos[centerIndex]};
    Punto<T> prevP{m_puntos[prevIndex]};
    Punto<T> nextP{m_puntos[nextIndex]};

    T firstCofactor{ crossProdValue(Vector<T>(centerP), Vector<T>(nextP))} ;
    T secondCofactor{ prevP.getX() * crossProdValue(Vector<T>(1,centerP.getY()),
                                                    Vector<T>(1, nextP.getY()))} ;
    T thirdCofactor{ prevP.getY() * crossProdValue(Vector<T>(1,centerP.getX()),
                                                   Vector<T>(1, nextP.getX()))};

    return firstCofactor - secondCofactor + thirdCofactor;
}

template<class T>
T Poligono<T>::doubleSignedArea() const {
    T area{ };
    for(int i{}; i < m_length; ++i)
    {
        int nexti{ (i+1)%m_length};
        T det{ crossProdValue(Vector<T>(m_puntos[i]), Vector<T>(m_puntos[nexti]))};
        area += det;
    }
    return area;
}


template<class T>
bool Poligono<T>::isCCW() const {
    return (doubleSignedArea() >= 0);
}

template<class T>
double Poligono<T>::area() const {
    return std::abs(doubleSignedArea())*0.5;
}

template<class T>
bool Poligono<T>::pointInside(const Punto<T> &p) const {
    int rightCrosses{ };

    for(int i{}; i < m_length; ++i)
    {
        T xAx{ p.getY() };
        int nexti{ (i+1)%m_length};
        Segmento<T> s{ m_puntos[i], m_puntos[nexti] };
        if (s.straddleHorizontally(xAx) || s.swapSegmento().straddleHorizontally(xAx))
        {
            double intersectX{ s.horizontalIntersect(xAx) };
            if (intersectX - p.getX() > 0)
            {
                ++rightCrosses;
            }
        }
    }
    // true if rightCrosses is odd (binary representation ends in 1)
    return (rightCrosses & 1);
}

template <class T>
std::ostream &operator<<(std::ostream &out, const Poligono<T> &pol) {
    out << "[";

    Punto<T>* ppunto{ pol.m_puntos };
    for(int i{ 0 }; i < pol.m_length; i++)
    {
        out << "v" << i << ": " << *ppunto;
        if (i != (pol.m_length-1))
        {
            out << ", ";
        }
        ++ppunto;
    }
    out << "]";
    return out;
}



#endif //ELEM_GEOMETRICOS_POLIGONO_H
