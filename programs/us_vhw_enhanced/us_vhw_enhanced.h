#ifndef US_VHW_ENHCD_H
#define US_VHW_ENHCD_H

#include "us_dataIO.h"
#include "us_analysis_base2.h"
#include "us_distrib_plot.h"
#include "us_editor.h"
#include "us_math2.h"
#include "us_model.h"
#include "us_run_details2.h"
#include "us_buffer_gui.h"

#include "qwt_plot_marker.h"

#define PA_POINTS 20     // plateau average points to left and right

#ifndef DbgLv
#define DbgLv(a) if(dbg_level>=a)qDebug() //!< debug-level-conditioned qDebug()
#endif

typedef struct groupinfo_s
{
   double        x1;          // x of top mouse pick
   double        y1;          // y of top mouse pick
   double        x2;          // x of bottom mouse pick
   double        y2;          // y of bottom mouse pick
   double        sed;         // average intercept sedcoeff of group
   double        percent;     // percent fraction of all divisions
   int           ndivs;       // number of division lines included in pick
   QList< int >  idivs;       // list of divisions (0 to n-1) included
} GrpInfo;

class US_vHW_Enhanced : public US_AnalysisBase2
{
   Q_OBJECT

   public:
      US_vHW_Enhanced();

   private:

      enum { NONE, START, END } groupstep;

      QLabel*       lb_tolerance;
      QLabel*       lb_division;

      QLineEdit*    le_temper;
      QLineEdit*    le_model;

      QCheckBox*    ck_modelpl;
      QCheckBox*    ck_vhw_enh;
      QCheckBox*    ck_manrepl;
      QCheckBox*    ck_use_fed;

      US_Help       showHelp;
 
      QwtCounter*   ct_tolerance;
      QwtCounter*   ct_division;

      QwtPlotCurve*  curve;
      QwtPlotCurve*  dcurve;
      US_PlotPicker* gpick;

      QPushButton*  pb_dstrpl;
      QPushButton*  pb_rsetda;
      QPushButton*  pb_selegr;
      QPushButton*  pb_replot;

      double        boundPct;
      double        positPct;
      double        baseline;
      double        correc;
      double        C0;
      double        c0term;
      double        Swavg;
      double        omega;
      double        range;
      double        span;
      double        basecut;
      double        platcut;
      double        bdtoler;
      double        invert;
      double        divfac;
      double        cpij;
      double        sumcpij;
      double        sedc;
      double        sdiff;
      double        bdiff_coef;
      double        bdiff_sedc;
      double        total_conc;

      int           row;
      int           run_id;
      int           divsCount;
      int           scanCount;
      int           valueCount;
      int           dbg_level;
      int           lscnCount;
      int           last_plot;                 // Last plot's triple row index

      bool          haveZone;
      bool          groupSel;
      bool          forcePlot;
      bool          skipPlot;
      bool          mo_plats;
      bool          vhw_enh;

      QString       run_name;
      QString       cell;
      QString       wavelength;
      QString       workingDir;
      QString       runID;
      QString       editID;

      QVector< US_DataIO::EditedData > dsimList;  // Simulation data
      QVector< bool >                  have_sims; // Have-simulation flags
      QVector< US_Model >              modlList;  // Models for triples

      QVector< double >            aseds;      // All division sedcoeff values
      QVector< double >            dseds;      // Division sedcoeff intercepts
      QVector< double >            dslos;      // Division slope values
      QVector< double >            dsigs;      // Division sigma values
      QVector< double >            dcors;      // Division correlation values
      QVector< int >               dpnts;      // Division fitted line points

      QVector< double >            scPlats;    // Scan plateaus current triple
      QVector< int >               liveScans;  // Vector of live-scan indexes
      QVector< QVector< double > > CPijs;      // CPij vecs, divs in scans
      QVector< QVector< double > > mconcs;     // Mid-div concs, divs in scans
      QVector< double >            bdrads;     // Back-diffusion radii
      QVector< double >            bdcons;     // Back-diffusion concentrations

      QList< double >              groupxy;    // Group select pick coordinates
      QList< GrpInfo >             groupdat;   // Selected group info structures

      US_DataIO::EditedData*       edata;      // Current triple edited data
      US_DataIO::EditedData*       expda;      // Current triple edited data (exp.)
      US_DataIO::EditedData*       simda;      // Current triple edited data (sim.)
      US_DataIO::Scan*             dscan;      // Current data scsan
      US_DataIO::Scan*             expsc;      // Current data scsan (exp.)
      US_DataIO::Scan*             simsc;      // Current data scsan (sim.)

      US_Model                     model;      // Current loaded model

      int kcalls[20]; // Timing counts
      int kmsecs[20];
   private slots:

      void load(        void );
      void data_plot(   void );
      void distr_plot(  void );
      void save_data(   void );
      void view_report( void );
      void sel_groups(  void );
      void div_seds(    void );
      void update_density(   double );
      void update_viscosity( double );
      void update_vbar(      double );
      void update_bdtoler(   double );
      void update_divis(     double );
      int  first_gteq( double, QVector< double >&, int, int );
      int  first_gteq( double, QVector< double >&, int );
      double sed_coeff( double, double, double* = NULL, int* = NULL );
      double avg_plateau(  void );
      double sedcoeff_intercept( void );
      double back_diff_coeff( double );
      void groupClick( const QwtDoublePoint& );
      void add_group_info( void );
      void write_vhw(      void );
      void write_dis(      void );
      void write_model(    void );
      void write_report(   QTextStream& );
      QString text_time( double, int );
      QString text_time( double );
      QStringList last_edit_files( QStringList );
      void new_triple  ( int );
      void update      ( int );
      void copy_data_files( QString, QString, QString );
      bool fitted_plateaus( void );
      bool model_plateaus ( void );
      void plot_refresh   ( void );
      void vhw_calcs_enhanced( double*, double* );
      void vhw_calcs_standard( double*, double* );
      bool have_model          ( void );
      void vert_exclude_lines  ( void );
      void exclude_from        ( double );
      void exclude_to          ( double );
      void live_scans          ( void );
      void init_partials       ( void );
      void update_mid_concs    ( void );
      void create_simulation   ( void );
      void plot_data2          ( void );
      void get_model           ( void );

      void help     ( void )
      { showHelp.show_help( "vhw_enhanced.html" ); };

};
#endif
