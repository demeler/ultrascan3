//! \file us_plot3d.h
#ifndef US_PLOT3D_H
#define US_PLOT3D_H

#include "us_extern.h"
#include "us_model.h"
#include "us_dataIO.h"
#include "us_plot.h"
#include "us_widgets_dialog.h"
#include "us_help.h"
#include "qwt3d_surfaceplot.h"
#include "qwt3d_function.h"
#include "qwt3d_plot.h"

#ifndef DbgLv
#define DbgLv(a) if(dbg_level>=a)qDebug() //!< debug-level-conditioned qDebug()
#endif

//! \brief A class to provide a window for a qwtplot3d surface plot

class US_GUI_EXTERN US_Plot3D : public QMainWindow
{
   Q_OBJECT

   public:
      //! \brief Contructor for surface plot class
      //! \param p  A pointer to the parent widget of this one
      //! \param m  A pointer to the model whose data is to be plotted
      US_Plot3D( QWidget* = NULL, US_Model* = NULL );

      //! \brief Public function to set 3 coordinate type flags
      //! \param tx  The type flag for X (1=MW, 2=s, ...)
      //! \param ty  The type flag for Y
      //! \param tz  The type flag for Z
      void setTypes     ( int, int, int );
      //! \brief Public function to set plot control parameters
      //! \param z_scale  Z scale factor
      //! \param a_gridr  Grid resolution
      //! \param a_alpha  Alpha factor
      //! \param a_beta   Beta factor
      //! \param x_scale  Relative X scale factor
      //! \param y_scale  Relative Y scale factor
      void setParameters( double, double, double, double,
                          double = 1.0, double = 1.0 );
      //! \brief Public function to (re)calculate Z values at fixed increments
      //! \param zdat     Z data vector of vectors
      void calculateData( QVector< QVector< double > >& );
      //! \brief Public function to replot the 3D data
      void replot       ( void );
      //! \brief Public function to return the data widget pointer
      //! \return Pointer to the GL data widget
      QGLWidget* dataWidgetP( void );
      //! \brief Public function to save the plot to a file
      //! \param filename  The full path to the file to create
      //! \param imagetype The image file type ("png" or "jpg")
      //! \return          Flag if save was successful
      bool save_plot( const QString, const QString );


   private:
      QWidget*      centralWidget;
      QGridLayout*  mainLayout;
      QVBoxLayout*  rightLayout;
      QHBoxLayout*  bottomLayout;
      QGridLayout*  rsliderLayout;
      QGridLayout*  bsliderLayout;

      QSlider*      poffsSlider;
      QSlider*      resolSlider;
      QSlider*      normqSlider;
      QSlider*      normlSlider;

      QAction*      openAct;
      QAction*      openffAct;
      QAction*      saveAct;
      QAction*      movieAct;
      QAction*      exitAct;
      QAction*      frameAct;
      QAction*      boxAct;
      QAction*      noneAct;
      QAction*      gridfrAct;
      QAction*      gridbAct;
      QAction*      gridrAct;
      QAction*      gridlAct;
      QAction*      gridcAct;
      QAction*      gridfAct;
      QAction*      scattdAct;
      QAction*      wirefrAct;
      QAction*      hiddlnAct;
      QAction*      polygnAct;
      QAction*      fdmeshAct;
      QAction*      nodataAct;
      QAction*      fldataAct;
      QAction*      flisolAct;
      QAction*      flemptAct;
      QAction*      normsAct;
      QAction*      coaxesAct;
      QAction*      cobackAct;
      QAction*      comeshAct;
      QAction*      conumbAct;
      QAction*      colablAct;
      QAction*      cocaptAct;
      QAction*      codataAct;
      QAction*      corsetAct;
      QAction*      fnnumbAct;
      QAction*      fnaxesAct;
      QAction*      fncaptAct;
      QAction*      fnrsetAct;
      QAction*      iconAct;
      QAction*      qwtpltAct;

      QActionGroup* frameGroup;
      QActionGroup* gridGroup;
      QActionGroup* dataGroup;
      QActionGroup* floorGroup;

      QMenu*        fileMenu;
      QMenu*        colorMenu;
      QMenu*        fontMenu;

      QToolBar*     fileToolBar;

      QComboBox*    cb_ifmt;

      QPushButton*  pb_std;
      QPushButton*  pb_light;

      QCheckBox*    ck_light;
      QCheckBox*    ck_ortho;
      QCheckBox*    ck_legend;
      QCheckBox*    ck_autosc;
      QCheckBox*    ck_mouse;
      QCheckBox*    ck_shade;

      QFont         titleFont;

      bool          have_ed;
      bool          skip_plot;

      int           nrows;
      int           ncols;
      int           typex;
      int           typey;
      int           typez;
      int           powrx;
      int           powry;
      int           redrawWait;
      int           dbg_level;

      double        xmin;
      double        xmax;
      double        ymin;
      double        ymax;
      double        zmin;
      double        zmax;
      double        zfloor;
      double        zscale;
      double        xscale;
      double        yscale;
      double        gridres;
      double        x_norm;
      double        y_norm;
      double        z_norm;
      double        x_scale;
      double        y_scale;
      double        z_scale;
      double        alpha;
      double        beta;

      QString       xatitle;
      QString       yatitle;
      QString       zatitle;

      QFrame*       frame;

      QTimer*       timer;

      QVector< QVector< double > > zdata;

      Qwt3D::SurfacePlot*          dataWidget;

   protected:
      US_Model*     model;     //!< model to plot in 3 dimensions

   signals:
      void    has_closed( void );

   protected slots:
      void    closeEvent( QCloseEvent* );

   private slots:

      void    std_button(   void );
      void    light_button( void );
      void    ifmt_chosen(  int  );
      void    light_check(  int  );
      void    ortho_check(  int  );
      void    legnd_check(  int  );
      void    autsc_check(  int  );
      void    mouse_check(  int  );
      void    shade_check(  int  );
      void    poffs_slide(  int  );
      void    resol_slide(  int  );
      void    norml_slide(  int  );
      void    normq_slide(  int  );
      void    movie_toggle(     bool );
      void    frame_axes_on(    bool );
      void    box_axes_on(      bool );
      void    no_axes_on(       bool );
      void    grid_front_on(    bool );
      void    grid_back_on(     bool );
      void    grid_left_on(     bool );
      void    grid_right_on(    bool );
      void    grid_ceil_on(     bool );
      void    grid_floor_on(    bool );
      void    set_grid_onoff(   Qwt3D::SIDE, bool );
      void    data_points_on(    bool );
      void    data_wirefr_on(    bool );
      void    data_hidden_on(    bool );
      void    data_polygn_on(    bool );
      void    data_fimesh_on(    bool );
      void    data_none_on(      bool );
      void    floor_data_on(     bool );
      void    floor_isol_on(     bool );
      void    floor_empty_on(    bool );
      void    normals_on(        bool );
      void    rotate(        void );
      void    open_file(     void );
      void    close_all(     void );
      void    pick_axes_co(  void );
      void    pick_back_co(  void );
      void    pick_mesh_co(  void );
      void    pick_numb_co(  void );
      void    pick_labl_co(  void );
      void    pick_capt_co(  void );
      void    pick_data_co(  void );
      void    reset_colors(  void );
      void    pick_numb_fn(  void );
      void    pick_axes_fn(  void );
      void    pick_capt_fn(  void );
      void    reset_fonts(   void );
      void    dump_contents( void );

      QString xyAxisTitle( int, double );
      QString zAxisTitle(  int );
      void    setStandardView( void );
      void    createActions(   void );
      void    createMenus(     void );
      void    createToolBar(   void );
      double  comp_value( US_Model::SimulationComponent*, int, double );

};
#endif

