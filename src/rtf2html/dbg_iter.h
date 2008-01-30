/*  This is RTF to HTML converter, implemented as a text filter, generally.
    Copyright (C) 2003 Valentin Lavrinenko, vlavrinenko@users.sourceforge.net

    available at http://rtf2html.sf.net

    Original available under the terms of the GNU LGPL2, and according
    to those terms, relicensed under the GNU GPL2 for inclusion in Tellico */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

namespace rtf {

template <class T>
class dbg_iter_mixin : public virtual T
{
 public:
   int offset;
   dbg_iter_mixin(const T& t) : T(t)
   {}
   T& operator=(const T& t)
   {
      return T::operator=(t);
   }
   dbg_iter_mixin& operator++ ()
   {
      ++offset;
      T::operator++();
      return *this;
   }
   dbg_iter_mixin operator++ (int i)
   {
      ++offset;
      return T::operator++(i);
   }
   char operator *() const
   {
      T::value_type c=T::operator*();
//      std::cerr<<offset<<":"<<c<<std::endl;
      return c;
   }
};

template <class T>
class dbg_iter : public dbg_iter_mixin<T>
{
 public:
   dbg_iter(const T& t) : dbg_iter_mixin<T>(t)
   {}
};

template<class T>
class dbg_iter<std::istreambuf_iterator<T> > :
   public virtual std::istreambuf_iterator<T>,
   public dbg_iter_mixin<std::istreambuf_iterator<T> >
{
 public:
   dbg_iter(std::basic_streambuf<T> *buf) : std::istreambuf_iterator<T>(buf)
   {}
};

}