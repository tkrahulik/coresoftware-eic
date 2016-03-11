#ifndef __PHPyOscarOutputManager_H__
#define __PHPyOscarOutputManager_H__

// $Id: PHPyOscarOutputManager.h,v 1.1 2011/08/05 17:47:36 pinkenbu Exp $

/*!
   \file PHPyOscarOutputManager.h
   \brief dedicated output manager to write phpythia contents into oscar formated text file
   \author Hugo Pereira
   \version $Revision: 1.1 $
   \date $Date: 2011/08/05 17:47:36 $
*/

#include <fun4all/Fun4AllOutputManager.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

class PHNodeIOManager;
class PHCompositeNode;
class PHPythiaContainer;

class PHPyOscarOutputManager: public Fun4AllOutputManager
{
  
  public:

  //! constructor
  PHPyOscarOutputManager(const std::string &myname = "OSCAROUT" , const std::string &filename = "oscar.txt");
  
  //! destructor
  virtual ~PHPyOscarOutputManager();

  virtual int outfileopen(const std::string &fname);
  virtual int Write(PHCompositeNode *startNode);

  protected:
  
  //! write full event to output stream
  /*!
  it can be called by derived classes.
  x, y and z_primary are the primary vertex position, stored in the oscar file as a geantino
  The unit is (mm)
  */
  virtual int _Write( 
    PHPythiaContainer*, 
    double x_primary_vertex, 
    double y_primary_vertex, 
    double z_primary_vertex );

  std::ofstream out;
  
};

#endif /* __PHPyOscarOutputManager_H__ */
