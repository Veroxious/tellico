/***************************************************************************
    copyright            : (C) 2007 by Sebastian Held
    email                : sebastian.held@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *    ### based on BaToo: http://people.inf.ethz.ch/adelmanr/batoo/ ###    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#ifndef BARCODE_H
#define BARCODE_H

#include <qthread.h>
#include <qimage.h>
#include <qvaluevector.h>
#include <qobject.h>
#include <math.h>

#include "barcode_v4l.h"

//#define BarcodeDecoder_DEBUG
//#define Decoder_EAN13_DEBUG
//#define Barcode_DEBUG

namespace barcodeRecognition {
	static int code_odd[][4] = { { 30, 20, 10, 10 }, 
		                        { 20, 20, 20, 10 }, 
		                        { 20, 10, 20, 20 }, 
		                        { 10, 40, 10, 10 }, 
		                        { 10, 10, 30, 20 }, 
		                        { 10, 20, 30, 10 }, 
		                        { 10, 10, 10, 40 }, 
		                        { 10, 30, 10, 20 }, 
		                        { 10, 20, 10, 30 }, 
		                        { 30, 10, 10, 20 } };

	static int code_even[][4] = { { 10, 10, 20, 30 }, 
		                         { 10, 20, 20, 20 }, 
		                         { 20, 20, 10, 20 }, 
		                         { 10, 10, 40, 10 }, 
		                         { 20, 30, 10, 10 }, 
		                         { 10, 30, 20, 10 }, 
		                         { 40, 10, 10, 10 }, 
		                         { 20, 10, 30, 10 }, 
		                         { 30, 10, 20, 10 }, 
		                         { 20, 10, 10, 30 } };

	static bool parity_pattern_list[][6] = { { false, false, false, false, false, false }, 
		                                       { false, false, true, false, true, true }, 
		                                       { false, false, true, true, false, true }, 
		                                       { false, false, true, true, true, false }, 
		                                       { false, true, false, false, true, true }, 
		                                       { false, true, true, false, false, true }, 
		                                       { false, true, true, true, false, false }, 
		                                       { false, true, false, true, false, true }, 
		                                       { false, true, false, true, true, false }, 
		                                       { false, true, true, false, true, false } };

  class Barcode_EAN13 {
  public:
    Barcode_EAN13();
    Barcode_EAN13( QValueVector<int> code );
    bool isNull() const { return m_null; }
    bool isValid() const;
    QValueVector<int> getNumbers() const;
    void setCode( QValueVector<int> code );
    QString toString() const;
    bool operator!= ( const Barcode_EAN13 &code );
  protected:
    QValueVector<int> m_numbers;
    bool m_null;
  };

  class MatchMakerResult {
  public:
    MatchMakerResult( bool even, int digit );
    bool isEven() const {return m_even;}
    int getDigit() const {return m_digit;}
  protected:
    int m_digit;
    bool m_even;
  };

  class Decoder_EAN13 {
  public:
    enum { BOTH_TABLES = 0, EVEN_TABLE = 1, ODD_TABLE = 2 };
    static Barcode_EAN13 recognize( QValueVector< QValueVector<int> > fields );
    static QValueVector<int> decode( QValueVector< QValueVector<int> > fields, int start_i, int end_i );
    static MatchMakerResult recognizeNumber( QValueVector< QValueVector<int> > fields, int code_table_to_use );
    static MatchMakerResult recognizeSystemCode( bool parity_pattern[6] );
  };

  /** \brief this thread handles barcode recognition using webcams
   *  @author Sebastian Held <sebastian.held@gmx.de>
   */
  class barcodeRecognitionThread : public QObject, public QThread {
    Q_OBJECT
  public:
    barcodeRecognitionThread();
    ~barcodeRecognitionThread();
    virtual void run();
    void stop();
    void recognizeBarcode( QImage img );
    bool isWebcamAvailable();
  signals:
    void recognized( QString barcode );
    void gotImage( QImage &img );
  protected:
    volatile bool m_stop;
    QImage m_barcode_img;
    QMutex m_stop_mutex, m_barcode_img_mutex;
    barcode_v4l *m_barcode_v4l;

    Barcode_EAN13 recognize( QImage img );
    Barcode_EAN13 recognizeCode( QImage img, int x1, int x2, int y );
    void addNumberToPossibleNumbers( QValueVector<int> number, int possible_numbers[10][13][2], bool correct_code );
    void sortDigits( int possible_numbers[10][13][2] );
    Barcode_EAN13 extractBarcode( int possible_numbers[10][13][2] );
    QValueVector<int> transformPathToBW( QValueVector<QRgb> line);
    QValueVector< QValueVector<int> > extractFieldInformation( QValueVector<int> string );
    Barcode_EAN13 detectValidBarcode ( int possible_numbers[10][13][2], int max_amount_of_considered_codes );
    bool isValid( int numbers[13] );
    bool isValid( QValueVector<int> numbers );
    void printArray( int array[10][13][2], int level );
  };
}

#endif
