//! \file us_report_gui.cpp

#include "us_report_gui.h"
#include "us_settings.h"
#include "us_gui_settings.h"
#include "us_passwd.h"
#include "us_db2.h"
#include "us_investigator.h"
#include "us_images.h"

//#include "us_report_gmp.h"


#if QT_VERSION < 0x050000
#define setSymbol(a)      setSymbol(*a)
#define setSamples(a,b,c) setData(a,b,c)
#define QRegularExpression(a)  QRegExp(a)
#endif


// Report GUI
//ALEXEY_NEW_REPORT: an arg now is QMap < QString( wvl ), US_ReportGMP > ?  
//US_ReportGui::US_ReportGui( US_ReportGMP *tmp_report ) : US_Widgets()
US_ReportGui::US_ReportGui( QMap < QString, US_ReportGMP* > report_map ) : US_Widgets()
{
  this->report_map           = report_map; 

  QList < QString > report_map_keys = report_map.keys();
  QStringList wvl_passed;
  for ( int i=0; i < report_map_keys.size(); ++i )
    wvl_passed << report_map_keys[ i ];

  qDebug() << "Size of report_map_keys -- " << report_map_keys.size();
  qDebug() << "Wvl_passed to ReportGui -- " << wvl_passed << ", wvl_passed[0].toInt() -- " << wvl_passed[0].toInt();

  if ( wvl_passed.size() == 1 && !wvl_passed[0].toInt() )
    wvl_passed.clear();
  
  int init_index = 0; // 1st in a QMap;
    
  this->report               = report_map[ report_map_keys[ init_index ] ]; 
  this->report_copy_original = *report;

  //Copy original QMap in case of cancelling updates:
  QMap < QString, US_ReportGMP* >::iterator ri;
  for ( ri = report_map.begin(); ri != report_map.end(); ++ri )
    {
      QString wvl = ri.key();
      
      report_map_copy_original[ wvl ] = *( ri.value() );
    }
  /////////////////////////////////////////////////

  setWindowTitle( tr( "Channel Report Editor"));
  
  //setPalette( US_GuiSettings::frameColor() );
  setPalette( US_GuiSettings::normalColor() );
  main  = new QVBoxLayout( this );
  main->setSpacing( 2 );
  main->setContentsMargins( 2, 2, 2, 2 );

  //Top level parameters
  QLabel* bn_report     = us_banner( QString( tr( "Report Parameters for channel: %1" ) ).arg( report->channel_name ), 1 );
  bn_report->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
  main->addWidget( bn_report );

  params    =  new QGridLayout();
  params    ->setSpacing         ( 2 );
  params    ->setContentsMargins ( 2, 2, 2, 2 );

  QLabel* lb_tot_conc       = us_label( tr( "Total Concentration" ) );
  QLabel* lb_tot_conc_tol   = us_label( tr( "Tolerance (+/-%)" ) );
  QLabel* lb_rmsd_limit     = us_label( tr( "RMSD (upper limit)" ) );
  QLabel* lb_av_intensity   = us_label( tr( "Minimum Intensity" ) );
  QLabel* lb_duration       = us_label( tr( "Experiment Duration" ) );
  QLabel* lb_duration_tol   = us_label( tr( "Tolerance (+/-%)" ) );
  QLabel* lb_wvl            = us_label( tr( "Wavelength" ) );
  QHBoxLayout* lo_duratlay  = us_ddhhmmsslay( 0, 0,0,0,1, &sb_durat_dd, &sb_durat_hh, &sb_durat_mm, &sb_durat_ss ); // ALEXEY 0 - visible, 1 - hidden

  le_tot_conc      = us_lineedit( QString::number(report->tot_conc),     0, false  );
  
  le_tot_conc_tol  = us_lineedit( QString::number(report->tot_conc_tol),     0, false  );
  le_duration_tol  = us_lineedit( QString::number(report->experiment_duration_tol),     0, false  );
  
  le_rmsd_limit    = us_lineedit( QString::number(report->rmsd_limit),   0, false  );
  le_av_intensity  = us_lineedit( QString::number(report->av_intensity), 0, false  );
  if ( report->channel_name.contains("Interf.") )
    le_av_intensity->setEnabled( false );

  le_tot_conc      -> setObjectName( "tot_conc" );
  le_rmsd_limit    -> setObjectName( "rmsd" );
  le_av_intensity  -> setObjectName( "av_intensity" );
  
  le_tot_conc_tol  -> setObjectName( "tot_conc_tol" );
  le_duration_tol  -> setObjectName( "duration_tol" );

  //set connecitons btw textChanged() and slot
  connect( le_tot_conc,   SIGNAL( textChanged ( const QString& ) ),
	   this,          SLOT  ( verify_text ( const QString& ) ) );
  connect( le_rmsd_limit, SIGNAL( textChanged ( const QString& ) ),
	   this,          SLOT  ( verify_text ( const QString& ) ) );
  connect( le_av_intensity, SIGNAL( textChanged ( const QString& ) ),
	   this,          SLOT  ( verify_text ( const QString& ) ) );
 connect( le_tot_conc_tol, SIGNAL( textChanged ( const QString& ) ),
	   this,          SLOT  ( verify_text ( const QString& ) ) );
  connect( le_duration_tol, SIGNAL( textChanged ( const QString& ) ),
	   this,          SLOT  ( verify_text ( const QString& ) ) );  
   
  qDebug() << "Report params on load: tot_conc, conc_tol, duraiton, duration_tol -- "
	   <<  report->tot_conc
	   <<  report->tot_conc_tol
	   <<  report->experiment_duration
	   <<  report->experiment_duration_tol;
  
  QList< int > dhms_dur;
  double exp_dur = report->experiment_duration;
  US_RunProtocol::timeToList( exp_dur, dhms_dur );
  sb_durat_dd ->setValue( (int)dhms_dur[ 0 ] );
  sb_durat_hh ->setValue( (int)dhms_dur[ 1 ] );
  sb_durat_mm ->setValue( (int)dhms_dur[ 2 ] );
  sb_durat_ss ->setValue( (int)dhms_dur[ 3 ] );

  //Connect Exp. Duration counters to changes:
  connect( sb_durat_dd,  SIGNAL( valueChanged   ( int ) ),
	   this,         SLOT  ( ssChgDuratTime_dd  ( int ) ) );
  connect( sb_durat_hh,  SIGNAL( valueChanged   ( int ) ),
	   this,         SLOT  ( ssChgDuratTime_hh ( int ) ) );
  connect( sb_durat_mm,  SIGNAL( valueChanged   ( int ) ),
	   this,         SLOT  ( ssChgDuratTime_mm ( int ) ) );
  connect( sb_durat_ss,  SIGNAL( valueChanged   ( int ) ),
	   this,         SLOT  ( ssChgDuratTime_ss ( int ) ) );

  //ALEXEY_NEW_REPORT: wvl
  cb_wvl =  us_comboBox();
  cb_wvl -> addItems( wvl_passed );
  cb_wvl -> setCurrentIndex( init_index );
  connect( cb_wvl,  SIGNAL( currentIndexChanged( int ) ),
            this,   SLOT  ( changeWvl          ( int ) ) );
  /////////////////////////////////////////////////////////////////////////////

  pb_prev_wvl     = us_pushbutton(  tr( "previous" ), true, 0 );
  pb_next_wvl     = us_pushbutton(  tr( "next"     ), true, 0 );
  pb_prev_wvl     ->setIcon( US_Images::getIcon( US_Images::ARROW_LEFT ) );
  pb_next_wvl     ->setIcon( US_Images::getIcon( US_Images::ARROW_RIGHT ) );

  connect( pb_prev_wvl, SIGNAL( clicked    () ),
	   this,        SLOT  ( wvl_prev   () ) );
  connect( pb_next_wvl, SIGNAL( clicked    () ),
	   this,        SLOT  ( wvl_next   () ) );
  ////////////////////////////////////////////////////////////////////////////

  pb_apply_all   = us_pushbutton(  tr( "Apply to all wvls" ), true, 0 );
  connect( pb_apply_all, SIGNAL( clicked          () ),
	   this,         SLOT  ( apply_all_wvls   () ) );


  ////////////////////////////////////////////////////////////////////////////
  
  row = 0;
  params->addWidget( lb_tot_conc,       row,    0, 1, 2 );
  params->addWidget( le_tot_conc,       row,    2, 1, 2 );
  
  params->addWidget( lb_tot_conc_tol,   row,    4, 1, 2 );
  params->addWidget( le_tot_conc_tol,   row,    6, 1, 2 );

  params->addWidget( lb_rmsd_limit,     row,    8, 1, 2 );
  params->addWidget( le_rmsd_limit,     row,    10, 1, 2 );

  params->addWidget( lb_wvl,            row,    12, 1, 2 );
  params->addWidget( cb_wvl,            row++,  14, 1, 2 );

  params->addWidget( lb_duration,       row,    0, 1, 2 );
  params->addLayout( lo_duratlay,       row,    2, 1, 2 );

  params->addWidget( lb_duration_tol,   row,    4, 1, 2 );
  params->addWidget( le_duration_tol,   row,    6, 1, 2 );  

  params->addWidget( lb_av_intensity,   row,    8, 1, 2 );
  params->addWidget( le_av_intensity,   row,    10, 1, 2 );

  params->addWidget( pb_prev_wvl,       row,    12, 1, 2 );
  params->addWidget( pb_next_wvl,       row++,  14, 1, 2 );
  params->addWidget( pb_apply_all,      row++,  14, 1, 2 );
  
  // int ihgt        = le_tot_conc->height();
  // QSpacerItem* spacer1 = new QSpacerItem( 20, 0.75*ihgt, QSizePolicy::Expanding );
  // params->setRowStretch( row, 1 );
  // params->addItem( spacer1,  row++,  0, 1, 1 );

  topContainerWidget = new QWidget;
  topContainerWidget->setLayout( params );
  main->addWidget( topContainerWidget );

  ////////////////////////////////////////////////////////////////////////////
  if ( report_map_keys.size() == 1) //single-wvl
    {
      pb_prev_wvl  ->setEnabled( false );
      pb_next_wvl  ->setEnabled( false );
      pb_apply_all ->setEnabled( false );
      
      pb_prev_wvl  ->hide();
      pb_next_wvl  ->hide();
      pb_apply_all ->hide();

      cb_wvl ->setEnabled( false );       
    }

  /////////////////////////////////////////////////////////////////////////////
  if ( init_index == 0 )
    pb_prev_wvl  ->setEnabled( false );
  if ( (init_index + 1 ) == cb_wvl->count() )
    pb_next_wvl  ->setEnabled( false );
  
  //main->addLayout( params );

  //Banner for Table
  QLabel* bn_report_t     = us_banner( tr( "Report Parameters: Type | Method" ) );
  bn_report_t->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
  main->addWidget( bn_report_t );

  //Main Table
  genL               = NULL;
  addRem_buttons     = NULL;
  reportmask         = NULL;
  lower_buttons      = NULL;

  build_report_layout();

}

//Exp. Durat. counters
void US_ReportGui::ssChgDuratTime_dd( int val )
{
  report->exp_time_changed = true;
}
void US_ReportGui::ssChgDuratTime_hh( int val )
{
  report->exp_time_changed = true;
}
void US_ReportGui::ssChgDuratTime_mm( int val )
{
  report->exp_time_changed = true;
}
void US_ReportGui::ssChgDuratTime_ss( int val )
{
  report->exp_time_changed = true;
}

//Layout build
void US_ReportGui::build_report_layout( void )
{
  qDebug() << "Building Tabular Layout -- ";
  //Clean genL layout first:
  if ( genL != NULL && genL->layout() != NULL )
    {
      QLayoutItem* item;
      while ( ( item = genL->layout()->takeAt( 0 ) ) != NULL )
	{
	  delete item->widget();
	  delete item;
	}
      delete genL;
      delete scrollArea;
    }
  //End cleaning layout
  
  int r_item_num = report->reportItems.size();
  qDebug() << "Size of report class -- " << report->reportItems.size();

  genL        =  new QGridLayout();
  genL        ->setSpacing         ( 2 );
  genL        ->setContentsMargins ( 2, 2, 2, 2 );

  //Table Header
  QLabel* lb_type    = us_label( tr( "Type" ) );
  QLabel* lb_method  = us_label( tr( "Method" ) );
  QLabel* lb_low     = us_label( tr( "Range Low" ) );
  QLabel* lb_high    = us_label( tr( "Range High" ) );
  QLabel* lb_intval  = us_label( tr( "Integration Value" ) );
  QLabel* lb_tol     = us_label( tr( "Tolerance (%)" ) );
  QLabel* lb_total   = us_label( tr( "Fraction of Total (%)" ) );
    
  row = 0;
  genL->addWidget( lb_type,   row,    0, 1, 2 );
  genL->addWidget( lb_method, row,    3, 1, 2 );
  genL->addWidget( lb_low,    row,    5, 1, 2 );
  genL->addWidget( lb_high,   row,    7, 1, 2 );
  genL->addWidget( lb_intval, row,    9, 1, 2 );
  genL->addWidget( lb_tol,    row,    11, 1, 2 );
  genL->addWidget( lb_total,  row++,  13, 1, 2 );
  //End of table header
  
  QComboBox* cb_type;  
  QComboBox* cb_method;
  QStringList sl_types;
  QStringList sl_methods;
  sl_types     << QString("s") << QString("D") << QString("f/f0") << QString("MW");
  sl_methods   << QString("2DSA-IT") << QString("PCSA-SL/DS/IS") << QString("2DSA-MC") << QString("raw");
  qDebug() << "Begin ReportItems iteration -- ";
   
  QLineEdit* le_low;    
  QLineEdit* le_high;   
  QLineEdit* le_intval; 
  QLineEdit* le_tol;    
  QLineEdit* le_total;
    
  for ( int ii = 0; ii < r_item_num; ii++ )
    {
      US_ReportGMP::ReportItem curr_item = report->reportItems[ ii ];
      
      //type ComboBox
      cb_type   =  us_comboBox();
      cb_type   -> clear();
      cb_type   -> addItems( sl_types );
      //need to set index corr. to type in ReportItem
      int type_ind = cb_type->findText( curr_item.type );
      cb_type->setCurrentIndex( type_ind );
      
      //method ComboBox
      cb_method = us_comboBox();      
      cb_method->clear();
      cb_method->addItems( sl_methods );
      //need to set index corr. to method in ReportItem
      int method_ind = cb_method->findText( curr_item.method );
      cb_method->setCurrentIndex( method_ind );
      
      le_low        = us_lineedit( QString::number(curr_item.range_low),  0, false  );
      le_high       = us_lineedit( QString::number(curr_item.range_high), 0, false  );
      le_intval     = us_lineedit( QString::number(curr_item.integration_val), 0, false  );
      le_tol        = us_lineedit( QString::number(curr_item.tolerance), 0, false  );
      le_total      = us_lineedit( QString::number(curr_item.total_percent), 0, true  );
      
      //set Object Name based on row number
      QString stchan      =  QString::number( ii ) + ": ";
      cb_type      -> setObjectName( stchan + "type" );
      cb_method    -> setObjectName( stchan + "method" );
      le_low       -> setObjectName( stchan + "low" );
      le_high      -> setObjectName( stchan + "high" );
      le_intval    -> setObjectName( stchan + "intval" );
      le_tol       -> setObjectName( stchan + "tol" );
      le_total     -> setObjectName( stchan + "total" );
      
      //set connecitons btw textChanged() and slot
      connect( le_low, SIGNAL( textChanged ( const QString& ) ),
	       this,   SLOT  ( verify_text ( const QString& ) ) );
      connect( le_high, SIGNAL( textChanged ( const QString& ) ),
	       this,   SLOT  ( verify_text ( const QString& ) ) );
      connect( le_intval, SIGNAL( textChanged ( const QString& ) ),
	       this,   SLOT  ( verify_text ( const QString& ) ) );
      connect( le_tol, SIGNAL( textChanged ( const QString& ) ),
	       this,   SLOT  ( verify_text ( const QString& ) ) );
      connect( le_total, SIGNAL( textChanged ( const QString& ) ),
	       this,   SLOT  ( verify_text ( const QString& ) ) );
      
      genL->addWidget( cb_type,   row,    0, 1, 2 );
      genL->addWidget( cb_method, row,    3, 1, 2 );
      genL->addWidget( le_low,    row,    5, 1, 2 );
      genL->addWidget( le_high,   row,    7, 1, 2 );
      genL->addWidget( le_intval, row,    9, 1, 2 );
      genL->addWidget( le_tol,    row,    11, 1, 2 );
      genL->addWidget( le_total,  row++,  13, 1, 2 );
    }
  
  int ihgt        = lb_low->height();
  QSpacerItem* spacer2 = new QSpacerItem( 20, 1*ihgt, QSizePolicy::Expanding);
  genL->setRowStretch( row, 1 );
  genL->addItem( spacer2,  row++,  0, 1, 1 );

  //add Scroll Area to genL
  scrollArea      = new QScrollArea;
  containerWidget = new QWidget;
  containerWidget->setLayout( genL );
  scrollArea     ->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  scrollArea     ->setWidgetResizable( true );
  scrollArea     ->setWidget( containerWidget );
  main->addWidget( scrollArea );

  //main->addLayout( genL );

  //Build | Re-build Add/Remove buttons layout
  qDebug() << "Building Add|Remove Buttons Layout -- ";
  //Clean layout first:
  if ( addRem_buttons != NULL && addRem_buttons->layout() != NULL )
    {
      QLayoutItem* item;
      while ( ( item = addRem_buttons->layout()->takeAt( 0 ) ) != NULL )
	{
	  delete item->widget();
	  delete item;
	}
      delete addRem_buttons;
    }
  
  addRem_buttons     = new QGridLayout();
  
  pb_addRow     = us_pushbutton( tr( "Add New Row" ) );
  connect( pb_addRow, SIGNAL( clicked() ), this, SLOT( add_row()  ) );

  pb_removeRow  = us_pushbutton( tr( "Remove Last Row" ) );
  connect( pb_removeRow, SIGNAL( clicked() ), this, SLOT( remove_row()  ) );
  if ( !report->channel_name.contains("Interf.") )
    {
      if (  report->reportItems.size() < 2   )
	pb_removeRow->setEnabled( false );
    }
  else
    {
      if (  report->reportItems.size() < 1   )
	pb_removeRow->setEnabled( false );
    }
  
  addRem_buttons->addWidget( pb_removeRow, row,  11, 1, 2 );
  addRem_buttons->addWidget( pb_addRow,    row,  13, 1, 2 );

  main->addLayout( addRem_buttons );
   

  //Build Report Mask header && button
  qDebug() << "Building Report Mask header and  Buttons Layout -- ";
  //Clean genL layout first:
  if ( reportmask != NULL && reportmask->layout() != NULL )
    {
      QLayoutItem* item;
      while ( ( item = reportmask->layout()->takeAt( 0 ) ) != NULL )
	{
	  delete item->widget();
	  delete item;
	}
      delete reportmask;
    }
  //End cleaning layout
  reportmask     =  new QGridLayout();
  reportmask     ->setSpacing         ( 2 );
  reportmask     ->setContentsMargins ( 2, 2, 2, 2 );
    
  //Banner
  QLabel* bn_repmask_t     = us_banner( tr( "Report Profile (Mask) Settings" ) );
  bn_repmask_t->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

  ck_tot_conc     = new QCheckBox( tr( "Total Concentration" ), this );
  ck_tot_conc ->setPalette( US_GuiSettings::normalColor() );
  ck_tot_conc ->setChecked( report-> tot_conc_mask );
  ck_tot_conc ->setAutoFillBackground( true  );

  ck_rmsd     = new QCheckBox( tr( "RMSD (upper limit)" ), this );
  ck_rmsd ->setPalette( US_GuiSettings::normalColor() );
  ck_rmsd ->setChecked( report-> rmsd_limit_mask );
  ck_rmsd ->setAutoFillBackground( true  );  

  ck_exp_duration     = new QCheckBox( tr( "Experiment Duration" ), this );
  ck_exp_duration ->setPalette( US_GuiSettings::normalColor() );
  ck_exp_duration ->setChecked( report-> experiment_duration_mask );
  ck_exp_duration ->setAutoFillBackground( true  );  
  
  ck_min_intensity     = new QCheckBox( tr( "Minimum Intensity" ), this );
  ck_min_intensity ->setPalette( US_GuiSettings::normalColor() );
  ck_min_intensity ->setChecked( report -> av_intensity_mask  );
  ck_min_intensity ->setAutoFillBackground( true  );  
  
  ck_integration     = new QCheckBox( tr( "Integration Results" ), this );
  ck_integration ->setPalette( US_GuiSettings::normalColor() );
  ck_integration ->setChecked( report -> integration_results_mask );
  ck_integration ->setAutoFillBackground( true  );

  row = 0;
  reportmask->addWidget( bn_repmask_t,     row++,  0, 1, 6 );
  reportmask->addWidget( ck_tot_conc,      row,    0, 1, 2 );
  reportmask->addWidget( ck_rmsd,          row,    2, 1, 2 );
  reportmask->addWidget( ck_exp_duration,  row++,  4, 1, 2 );
  reportmask->addWidget( ck_min_intensity, row,    0, 1, 2 );
  reportmask->addWidget( ck_integration,   row,    2, 1, 2 );
  
  main->addLayout( reportmask );
  
  //Build | Re-build Lower buttons layout
  qDebug() << "Building Lower Buttons Layout -- ";
  //Clean genL layout first:
  if ( lower_buttons != NULL && lower_buttons->layout() != NULL )
    {
      QLayoutItem* item;
      while ( ( item = lower_buttons->layout()->takeAt( 0 ) ) != NULL )
	{
	  delete item->widget();
	  delete item;
	}
      delete lower_buttons;
    }
  //End cleaning layout
  lower_buttons     = new QHBoxLayout();

  pb_cancel   = us_pushbutton( tr( "Cancel" ) );
  pb_accept   = us_pushbutton( tr( "Accept" ) );
  
  connect( pb_cancel, SIGNAL( clicked() ), this, SLOT( cancel_update() ) );
  connect( pb_accept, SIGNAL( clicked() ), SLOT( update_report() ) );

  lower_buttons->addWidget( pb_cancel );
  lower_buttons->addWidget( pb_accept );
   
  main->addLayout( lower_buttons );

  setMinimumSize( 850, 450 );
  //adjustSize();

}

//Slot to call Report Mask Gui
void US_ReportGui::report_mask( void )
{
  
}

//Slot to update report
void US_ReportGui::update_report( void )
{
  //Check for errors
  int syntax_errors = check_syntax();
  if ( syntax_errors != 0 )
    {
      QString mtitle_error    = tr( "Error" );
      QString message_error   = QString( tr( "There are %1 syntax errors!"
					     "\n\nPlease fix them (red fields) before accepting..." ))
	                        .arg( QString::number( syntax_errors ) );
      QMessageBox::critical( this, mtitle_error, message_error );
      return;
    }

  //save Gui params to reportGMP structure
  gui_to_report();

  /**
  //now ask if you want to apply all settings for current report to all other channels' reports
  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Information);
  msgBox.setWindowTitle(tr("Apply to Other Reports"));
  
  QString msg_text      = QString("Do you want to apply current report settings to all other channels?");
  msgBox.setText( msg_text );
    
  QPushButton *Accept    = msgBox.addButton(tr("Yes"), QMessageBox::YesRole);
  QPushButton *Cancel    = msgBox.addButton(tr("No"), QMessageBox::RejectRole);
  msgBox.exec();
  
  if ( msgBox.clickedButton() == Accept )
    {
      //Emit signal
      qDebug() << "Applying report settigns to all --";
      emit apply_to_all_reports( report );
      close();
    }
  else if (msgBox.clickedButton() == Cancel)
    close();
    return;
  *****/
  
  close();
}


//Verify text
void US_ReportGui::verify_text( const QString& text )
{
  QObject* sobj       = sender();      // Sender object
  QString oname       = sobj->objectName();
  
  qDebug() << "QLineEdit oname, text -- " << oname << text;
  QLineEdit * curr_widget = NULL;

  if ( oname.contains(":") )
    curr_widget = containerWidget->findChild<QLineEdit *>( oname );
  else
    curr_widget = topContainerWidget->findChild<QLineEdit *>( oname );
  // else if ( oname == "tot_conc" )
  //   curr_widget = le_tot_conc;
  // else if ( oname == "rmsd" )
  //   curr_widget = le_rmsd_limit;
  // else if ( oname == "av_intensity" )
  //   curr_widget = le_av_intensity;

  QRegExp rx_double("\\d*\\.?\\d+");

  if ( curr_widget != NULL )
    {      
      if ( !rx_double.exactMatch( text ) )
	{
	  QPalette *palette = new QPalette();
	  palette->setColor(QPalette::Text,Qt::white);
	  palette->setColor(QPalette::Base,Qt::red);
	  curr_widget->setPalette(*palette);

	  isErrorField[ oname ] = true;
	}
      else
	{
	  qDebug() << "Text verified!";
	  QPalette *palette = new QPalette();
	  palette->setColor(QPalette::Text,Qt::black);
	  palette->setColor(QPalette::Base,Qt::white);
	  curr_widget->setPalette(*palette);

	  isErrorField[ oname ] = false;

	  //ALEXEY: for a given row, compute 'Fraction of Total' (read-only) as '(Int_Val / Total Conc.) * 100%'
	  if ( oname.contains(": intval") )
	    {
	      double tot_conc_val = le_tot_conc ->text().toDouble();
	      if ( text.toDouble() > tot_conc_val ) 
		curr_widget -> setText( QString::number( tot_conc_val ) );
			      
	      QString item_row = oname.split(":")[0] + QString(": ");
	      QString objName_fraction_of_total  = item_row + "total";
	      QLineEdit * fraction_of_total_widget = containerWidget->findChild<QLineEdit *>( objName_fraction_of_total );

	      double fraction = ( curr_widget ->text().toDouble() / tot_conc_val ) * 100.0;
	      fraction_of_total_widget -> setText( QString::number( fraction ) );
	      palette->setColor( QPalette::Base, QColor( 0xe0, 0xe0, 0xe0 ) );
	      fraction_of_total_widget -> setPalette( *palette );
	    }
	  //ALEXEY: check the same for tot_conc: apply to all ReportItems' 'Fraction of Total'
	  if ( oname.contains("tot_conc") )
	    {
	      int r_item_num = report->reportItems.size();

	      qDebug() <<  "In Verify_text: report->reportItems.size() -- " << r_item_num;

	      for ( int ii = 0; ii < r_item_num; ii++ )
		{
		  QString objName_integration_val      = QString::number( ii ) + QString(": intval");
		  QString objName_fraction_of_total    = QString::number( ii ) + QString(": total");
		  QLineEdit * integration_val_widget   = containerWidget->findChild<QLineEdit *>( objName_integration_val );
		  QLineEdit * fraction_of_total_widget = containerWidget->findChild<QLineEdit *>( objName_fraction_of_total );
		  
		  double fraction = ( integration_val_widget ->text().toDouble() / text.toDouble() ) * 100.0;
		  fraction_of_total_widget -> setText( QString::number( fraction ) );
		  palette->setColor( QPalette::Base, QColor( 0xe0, 0xe0, 0xe0 ) );
		  fraction_of_total_widget -> setPalette( *palette );
		}
	    }
	}
    }
}

//Transfer GUI params to US_ReportGMP structure
void US_ReportGui::gui_to_report( void )
{
  //Main params
  report->tot_conc            = le_tot_conc     ->text().toDouble();
  report->rmsd_limit          = le_rmsd_limit   ->text().toDouble();
  report->av_intensity        = le_av_intensity ->text().toDouble();

  report->tot_conc_tol        = le_tot_conc_tol ->text().toDouble();
  report->experiment_duration_tol        = le_duration_tol ->text().toDouble();
  
  QList< int > dhms_dur;
  int t_day        = (double)sb_durat_dd->value();
  int t_hour       = (double)sb_durat_hh->value();
  int t_minute     = (double)sb_durat_mm->value();
  int t_second     = (double)sb_durat_ss->value();

  dhms_dur << t_day << t_hour << t_minute << t_second;

  qDebug() << "Gui-to-report: DURATION: dd hh mm ss -- " << t_day << t_hour << t_minute << t_second;
  
  double exp_dur = 0;
  US_RunProtocol::timeFromList( exp_dur, dhms_dur );
  report->experiment_duration = exp_dur;

  qDebug() << "Gui-to-report: DURATION: in seconds -- " << exp_dur;

  qDebug() << "Gui-to-report: reportItems.size()  -- " << report->reportItems.size();  


  //ReportItems
  for ( int ii = 0; ii < report->reportItems.size(); ii++ )
    {
      QString stchan      =  QString::number( ii ) + ": ";
      
      //type
      QComboBox * cb_type    = containerWidget->findChild<QComboBox *>( stchan + "type" );
      qDebug() << "ii, cb_type->currentText()" << ii << cb_type->currentText();
      report->reportItems[ ii ].type = cb_type->currentText();
      
      //method
      QComboBox * cb_method    = containerWidget->findChild<QComboBox *>( stchan + "method" );
      qDebug() << "ii, cb_method->currentText()" << ii << cb_method->currentText();
      report->reportItems[ ii ].method = cb_method->currentText();
      
      //range_low
      QLineEdit * le_low    = containerWidget->findChild<QLineEdit *>( stchan + "low" );
      qDebug() << "ii, le_low->text()" << ii << le_low->text();
      report->reportItems[ ii ].range_low = le_low->text().toDouble();

      //range_high
      QLineEdit * le_high    = containerWidget->findChild<QLineEdit *>( stchan + "high" );
      qDebug() << "ii, le_high->text()" << ii << le_high->text();
      report->reportItems[ ii ].range_high = le_high->text().toDouble();

      //integration value
      QLineEdit * le_intval  = containerWidget->findChild<QLineEdit *>( stchan + "intval" );
      qDebug() << "ii, le_intval->text()" << ii << le_intval->text();
      report->reportItems[ ii ].integration_val = le_intval->text().toDouble();
      
      //tolerance
      QLineEdit * le_tol  = containerWidget->findChild<QLineEdit *>( stchan + "tol" );
      qDebug() << "ii, le_tol->text()" << ii << le_tol->text();
      report->reportItems[ ii ].tolerance = le_tol->text().toDouble();
      
      //percent
      QLineEdit * le_total  = containerWidget->findChild<QLineEdit *>( stchan + "total" );
      qDebug() << "ii, le_total->text()" << ii << le_total->text();
      report->reportItems[ ii ].total_percent = le_total->text().toDouble();
    }

  //Report Mask params.
  report -> tot_conc_mask             = ck_tot_conc      ->isChecked();
  report -> rmsd_limit_mask           = ck_rmsd          ->isChecked();
  report -> av_intensity_mask         = ck_min_intensity ->isChecked();
  report -> experiment_duration_mask  = ck_exp_duration  ->isChecked();
  report -> integration_results_mask  = ck_integration   ->isChecked();

}

//Slot to cancel any updates on channel's report
void US_ReportGui::cancel_update( void )
{
  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setWindowTitle(tr("Cancel Updates"));
  
  //QString msg_text      = QString("Attention! Do you want to cancel all report updates for channel %1?").arg( channel_name );
  QString msg_text      = QString("Attention! Do you want to cancel all report updates for the current channel?");
  msgBox.setText( msg_text );
    
  QPushButton *Accept    = msgBox.addButton(tr("Yes"), QMessageBox::YesRole);
  QPushButton *Cancel    = msgBox.addButton(tr("No"), QMessageBox::RejectRole);
  msgBox.exec();
  
  if ( msgBox.clickedButton() == Accept )
    {
      // //Send copy of the report to restore to original
      // qDebug() << "Sizes: report CHANGED,  report_original -- " << report->reportItems.size() << report_copy_original.reportItems.size();
      
      // report = &( report_copy_original );

      // qDebug() << "Sizes: report RESTORED, report_original -- " << report->reportItems.size() << report_copy_original.reportItems.size();
      // qDebug() << "Report_copy_original.channel_name -- " << report_copy_original.channel_name;
      // emit cancel_changes( report_copy_original );

      emit cancel_changes( report_map_copy_original );
      close();
    }
  else if (msgBox.clickedButton() == Cancel)
    return;
}


//Slot to add row
void US_ReportGui::add_row( void )
{
  //Check if there are exsiting syntax errors:
  int syntax_errors = check_syntax();
  if ( syntax_errors != 0 )
    {
      QString mtitle_error    = tr( "Error" );
      QString message_error   = QString( tr( "There are %1 syntax errors!"
					     "\n\nPlease fix them (red fields) before adding new row..." ))
	                        .arg( QString::number( syntax_errors ) );
      QMessageBox::critical( this, mtitle_error, message_error );
      return;
    }

  //If no errors, save Gui params to reportGMP structure
  gui_to_report();

  //Add plain ReportItem
  US_ReportGMP::ReportItem initItem;

  if ( report-> channel_name . contains("Interf.") )
    {
      initItem.type             = QString("s");
      initItem.method           = QString("2DSA-IT");
      initItem.range_low        = 0;
      initItem.range_high       = 0;
      initItem.integration_val  = 0;
      initItem.tolerance        = 0;

      report->interf_report_changed = true;
    }
  else
    {
      initItem.type             = QString("s");
      initItem.method           = QString("2DSA-IT");
      initItem.range_low        = 3.2;
      initItem.range_high       = 3.7;
      initItem.integration_val  = 0.57;
      initItem.tolerance        = 10;
    }

  //Compute 'Fraction of Total' based on tot_conc:
  double tot_conc_val = le_tot_conc -> text().toDouble();
  initItem.total_percent    = ( initItem.integration_val / tot_conc_val) * 100.0;
  
  report->reportItems.push_back( initItem );

  //re-build genL layout
  build_report_layout( );
}

//Slot to remove rows
void US_ReportGui::remove_row( void )
{
  //Check if there are exsiting syntax errors:
  int syntax_errors = check_syntax();
  if ( syntax_errors != 0 )
    {
      QString mtitle_error    = tr( "Error" );
      QString message_error   = QString( tr( "There are %1 syntax errors!"
					     "\n\nPlease fix them (red fields) before removing row..." ))
	                        .arg( QString::number( syntax_errors ) );
      QMessageBox::critical( this, mtitle_error, message_error );
      return;
    }

  //If no errors, remove last row
  report->reportItems.removeLast();

  //save Gui params to reportGMP structure
  gui_to_report();
  
  //re-build genL layout
  build_report_layout( );
}

//Check systax errors in the fields
int US_ReportGui::check_syntax( void )
{
  int syntax_errors = 0;
  
  QMap<QString, bool>::iterator ri;
  for ( ri = isErrorField.begin(); ri != isErrorField.end(); ++ri )
    {
      if ( ri.value() )
	{
	  qDebug() << "Error for object name -- " << ri.key();
	  ++syntax_errors;
	}
    }

  return syntax_errors;
}

// Slot to select channel's wavelength     
void US_ReportGui::changeWvl( int ndx )
{
  //Check for errors
  int syntax_errors = check_syntax();
  if ( syntax_errors != 0 )
    {
      QString mtitle_error    = tr( "Error" );
      QString message_error   = QString( tr( "There are %1 syntax errors!"
					     "\n\nPlease fix them (red fields) before proceeding..." ))
	                        .arg( QString::number( syntax_errors ) );
      QMessageBox::critical( this, mtitle_error, message_error );
      return;
    }
  
   //first, save all changes for current | old (if clicked pb_prev | pb_next ) triple's report under consideration:
   gui_to_report(); 
  
   //cb_wvl->setCurrentIndex( ndx );
   QString curr_wvl    = cb_wvl ->itemText( ndx );

   qDebug() << "Current wvl -- " << curr_wvl;

   //set current report to that corresponding to the new wvl:
   this->report = report_map[ curr_wvl ];

   qDebug() << "ChangeWvl: switched to new report --";

   //At this point, we need to temporarily disconnect upper-portion GUi widgets from ::verify_Text()
   // As the new layout is not built yet...
   le_tot_conc     ->disconnect();
   le_rmsd_limit   ->disconnect();
   le_av_intensity ->disconnect();
   le_tot_conc_tol ->disconnect();
   le_duration_tol ->disconnect();

   //update upper portion of the Gui with the new values
   le_tot_conc     -> setText( QString::number(report->tot_conc) );
   qDebug() << "ChangeWvl: tot_conc set --";
   
   le_rmsd_limit   -> setText( QString::number(report->rmsd_limit) );
   qDebug() << "ChangeWvl: rmsd_lim set --";
   
   le_av_intensity -> setText( QString::number(report->av_intensity) );
   qDebug() << "ChangeWvl: av_intensity set --";
   
   le_tot_conc_tol -> setText( QString::number(report->tot_conc_tol) );
   qDebug() << "ChangeWvl: tot_conc_tol set --";
   
   le_duration_tol -> setText( QString::number(report->experiment_duration_tol) );
   qDebug() << "ChangeWvl: duration_tol set --";
   
   QList< int > dhms_dur;
   double exp_dur = report->experiment_duration;
   US_RunProtocol::timeToList( exp_dur, dhms_dur );
   sb_durat_dd ->setValue( (int)dhms_dur[ 0 ] );
   sb_durat_hh ->setValue( (int)dhms_dur[ 1 ] );
   sb_durat_mm ->setValue( (int)dhms_dur[ 2 ] );
   sb_durat_ss ->setValue( (int)dhms_dur[ 3 ] );

   //re-build genL layout ( lower portion, the reportItems )
   build_report_layout( );

   //Reconnect upper-portion Gui elements to ::verify_text()
   connect( le_tot_conc,   SIGNAL( textChanged ( const QString& ) ),
	    this,          SLOT  ( verify_text ( const QString& ) ) );
   connect( le_rmsd_limit, SIGNAL( textChanged ( const QString& ) ),
	    this,          SLOT  ( verify_text ( const QString& ) ) );
   connect( le_av_intensity, SIGNAL( textChanged ( const QString& ) ),
	    this,          SLOT  ( verify_text ( const QString& ) ) );
   connect( le_tot_conc_tol, SIGNAL( textChanged ( const QString& ) ),
	    this,          SLOT  ( verify_text ( const QString& ) ) );
   connect( le_duration_tol, SIGNAL( textChanged ( const QString& ) ),
	    this,          SLOT  ( verify_text ( const QString& ) ) );  
   
   //Next/Previous wvl btns
   if ( ndx == 0 )
     pb_prev_wvl->setEnabled( false );
   else
     pb_prev_wvl->setEnabled( true );
   
   if ( (ndx + 1) == cb_wvl->count() )
     pb_next_wvl->setEnabled( false );
   else
     pb_next_wvl->setEnabled( true );
    
}

// Slot to select next channel's wavelength     
void US_ReportGui::wvl_next( void )
{
  //Check for errors
  int syntax_errors = check_syntax();
  if ( syntax_errors != 0 )
    {
      QString mtitle_error    = tr( "Error" );
      QString message_error   = QString( tr( "There are %1 syntax errors!"
					     "\n\nPlease fix them (red fields) before proceeding..." ))
	                        .arg( QString::number( syntax_errors ) );
      QMessageBox::critical( this, mtitle_error, message_error );
      return;
    }
  ///////////////
  
  pb_prev_wvl ->setEnabled( true );
  int row = cb_wvl->currentIndex() + 1;

  if ( (row + 1 ) <= cb_wvl->count() )
    {
      cb_wvl->setCurrentIndex( row );
      
      if ( (row + 1 ) == cb_wvl->count() )
	pb_next_wvl ->setEnabled( false );
    }
  else
    pb_next_wvl->setEnabled( row < cb_wvl->count() );
}

// Slot to select previous channel's wavelength     
void US_ReportGui::wvl_prev( void )
{
  //Check for errors
  int syntax_errors = check_syntax();
  if ( syntax_errors != 0 )
    {
      QString mtitle_error    = tr( "Error" );
      QString message_error   = QString( tr( "There are %1 syntax errors!"
					     "\n\nPlease fix them (red fields) before proceeding..." ))
	                        .arg( QString::number( syntax_errors ) );
      QMessageBox::critical( this, mtitle_error, message_error );
      return;
    }
  ///////////////
  
  
  pb_next_wvl ->setEnabled( true );
  int row = cb_wvl->currentIndex() - 1;

  qDebug() << "Row: PREV clicked -- " << row;

    
  if ( row  >= 0 )
    {
      cb_wvl->setCurrentIndex( row );
      
      if ( row == 0 )
	pb_prev_wvl ->setEnabled( false );
      
    }
  else
    pb_prev_wvl ->setEnabled( false );
}

//Slot to apply current triple's settings to the rest of triples (wvls) in a channel
void US_ReportGui::apply_all_wvls( void )
{
  //Check for errors
  int syntax_errors = check_syntax();
  if ( syntax_errors != 0 )
    {
      QString mtitle_error    = tr( "Error" );
      QString message_error   = QString( tr( "There are %1 syntax errors!"
					     "\n\nPlease fix them (red fields) before proceeding..." ))
	                        .arg( QString::number( syntax_errors ) );
      QMessageBox::critical( this, mtitle_error, message_error );
      return;
    }
  ///////////////
    
  //now ask if you want to apply all settings for current report to all other channel's triples
  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Information);
  msgBox.setWindowTitle(tr("Apply to Other Channel's Triples"));
  
  QString msg_text      = QString("Do you want to apply current settings to all other triples?\n\n(Existing settings will be overwritten...)");
  msgBox.setText( msg_text );
    
  QPushButton *Accept    = msgBox.addButton(tr("Yes"), QMessageBox::YesRole);
  QPushButton *Cancel    = msgBox.addButton(tr("No"), QMessageBox::RejectRole);
  msgBox.exec();
  
  if ( msgBox.clickedButton() == Accept )
    {
      qDebug() << "Applying report settigns to all --";

      // apply gui params to current report structure:
      gui_to_report();
      
      QString current_report_key = cb_wvl ->currentText();
      qDebug() << "In apply_to_all: current_report_key, report->wavelength -- " << current_report_key << report->wavelength;  
      //iterate over channel's triples:
      QMap<QString, US_ReportGMP* >::iterator ri;
      for ( ri = report_map.begin(); ri != report_map.end(); ++ri )
	{
	  if ( ri.key().contains( current_report_key ) )
	    continue;
	  
	  report_map[ ri.key() ]->tot_conc            = report->tot_conc;
	  report_map[ ri.key() ]->rmsd_limit          = report->rmsd_limit;
	  report_map[ ri.key() ]->av_intensity        = report->av_intensity;
	  report_map[ ri.key() ]->experiment_duration = report->experiment_duration;

	  report_map[ ri.key() ]->tot_conc_tol        = report->tot_conc_tol;
	  report_map[ ri.key() ]->experiment_duration_tol   = report->experiment_duration_tol;

	  report_map[ ri.key() ]->tot_conc_mask              = report->tot_conc_mask;
	  report_map[ ri.key() ]->rmsd_limit_mask            = report->rmsd_limit_mask;
	  report_map[ ri.key() ]->av_intensity_mask          = report->av_intensity_mask;
	  report_map[ ri.key() ]->experiment_duration_mask   = report->experiment_duration_mask;
	  report_map[ ri.key() ]->integration_results_mask   = report->integration_results_mask;
	  
	  //Now go over reportItems:
	  //1st, clear current array of reportItems:
	  report_map[ ri.key() ]->reportItems.clear();
	  
	  for ( int ic = 0; ic < report->reportItems.size(); ++ic )
	    report_map[ ri.key() ]->reportItems.push_back( report->reportItems[ ic ] );
	}
      
    }
  else if (msgBox.clickedButton() == Cancel)
    return;
}
