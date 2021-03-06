#include "TimeSliderWidget.h"

#include <QtGui/QMenu>
#include <QtGui/QColorDialog>
#include <QtGui/QInputDialog>

#include <boost/assign/list_of.hpp>
#include <boost/format.hpp>

#include <algorithm> 

using namespace FabricSplice;
using namespace std;

TimeSliderWidget::TimeSliderWidget(QWidget * parent)
	: QWidget(parent)
{
  m_settingTime = false;

	setFixedHeight(40);
	
	// default direction is forward
	m_direction = 1;
	
	// default looping ( 0 )
	m_looping = 1;
	
	m_timer = new QTimer(this);
	m_timer->setInterval(0);

  // layout
  setLayout(new QHBoxLayout());

  // elements
  m_startSpinBox = new QDoubleSpinBox(this);
  m_startSpinBox->setMinimumWidth(60);
  m_startSpinBox->setMaximumWidth(60);
  m_startSpinBox->setWrapping(false);
  m_startSpinBox->setFrame(false);
  m_startSpinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  m_startSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
  m_startSpinBox->setDecimals(0);
  m_startSpinBox->setMinimum(-1000000.000000000000000);
  m_startSpinBox->setMaximum(1000000.000000000000000);
  layout()->addWidget(m_startSpinBox);

  m_frameSlider = new QSlider(this);
  m_frameSlider->setOrientation(Qt::Horizontal);
  m_frameSlider->setInvertedControls(false);
  m_frameSlider->setTickPosition(QSlider::TicksBelow);
  m_frameSlider->setTickInterval(10);
  m_frameSlider->setMinimum(0);
  m_frameSlider->setMaximum(1000);
  layout()->addWidget(m_frameSlider);

  m_endSpinBox = new QDoubleSpinBox(this);
  m_endSpinBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  m_endSpinBox->setMinimumWidth(60);
  m_endSpinBox->setMaximumWidth(60);
  m_endSpinBox->setFrame(false);
  m_endSpinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  m_endSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
  m_endSpinBox->setDecimals(0);
  m_endSpinBox->setMinimum(-1000000.000000000000000);
  m_endSpinBox->setMaximum(100000000.000000000000000);
  m_endSpinBox->setValue(1000.000000000000000);
  layout()->addWidget(m_endSpinBox);

  m_currentFrameSpinBox = new QDoubleSpinBox(this);
  m_currentFrameSpinBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  m_currentFrameSpinBox->setMinimumWidth(70);
  m_currentFrameSpinBox->setMaximumWidth(70);
  m_currentFrameSpinBox->setFrame(false);
  m_currentFrameSpinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  m_currentFrameSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
  m_currentFrameSpinBox->setDecimals(0);
  m_currentFrameSpinBox->setMinimum(-1000000.000000000000000);
  m_currentFrameSpinBox->setMaximum(100000000.000000000000000);
  m_currentFrameSpinBox->setValue(1000.000000000000000);
  layout()->addWidget(m_currentFrameSpinBox);

  m_goToStartFrameButton = new QPushButton(this);
  m_goToStartFrameButton->setMinimumWidth(40);
  m_goToStartFrameButton->setMaximumWidth(40);
  m_goToStartFrameButton->setText("|<<");
  layout()->addWidget(m_goToStartFrameButton);

  m_goToPreviousFrameButton = new QPushButton(this);
  m_goToPreviousFrameButton->setMinimumWidth(40);
  m_goToPreviousFrameButton->setMaximumWidth(40);
  m_goToPreviousFrameButton->setText("|<");
  layout()->addWidget(m_goToPreviousFrameButton);

  m_playButton = new QPushButton(this);
  m_playButton->setMinimumWidth(40);
  m_playButton->setMaximumWidth(40);
  m_playButton->setText(">");
  layout()->addWidget(m_playButton);

  m_goToNextFrameButton = new QPushButton(this);
  m_goToNextFrameButton->setMinimumWidth(40);
  m_goToNextFrameButton->setMaximumWidth(40);
  m_goToNextFrameButton->setText(">|");
  layout()->addWidget(m_goToNextFrameButton);

  m_goToEndFrameButton = new QPushButton(this);
  m_goToEndFrameButton->setMinimumWidth(40);
  m_goToEndFrameButton->setMaximumWidth(40);
  m_goToEndFrameButton->setText(">>|");
  layout()->addWidget(m_goToEndFrameButton);

  m_frameRateComboBox = new QComboBox(this);
  m_frameRateComboBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  m_frameRateComboBox->setMinimumWidth(80);
  m_frameRateComboBox->setMaximumWidth(80);
  m_frameRateComboBox->setFrame(false);
  m_frameRateComboBox->setLayoutDirection(Qt::LeftToRight);
  m_frameRateComboBox->setEditable(false);
  m_frameRateComboBox->addItem("max fps");
  m_frameRateComboBox->addItem("12 fps");
  m_frameRateComboBox->addItem("24 fps");
  m_frameRateComboBox->addItem("48 fps");
  layout()->addWidget(m_frameRateComboBox);

  m_loopingComBox = new QComboBox(this);
  m_loopingComBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  m_loopingComBox->setMinimumWidth(100);
  m_loopingComBox->setMaximumWidth(100);
  m_loopingComBox->setFrame(false);
  m_loopingComBox->setLayoutDirection(Qt::LeftToRight);
  m_loopingComBox->setEditable(false);
  m_loopingComBox->addItem("Loop Once");
  m_loopingComBox->addItem("Continuous");
  m_loopingComBox->addItem("Oscillate");
  layout()->addWidget(m_loopingComBox);

  // QLine * line = new QLine();
  // line->setOrientation(Qt::Vertical);
  // layout()->addWidget(line);

	// now we setup the connection 
	connect(m_frameSlider , SIGNAL(valueChanged(int)) , this , SLOT(sliderChanged(int)));

	connect(m_currentFrameSpinBox , SIGNAL(editingFinished()) , this , SLOT(currentFrameChanged()));

//  there is an issue with doing that when we enter a value manually
//  not doing it is also an issue because then changing the value with the wheel is not doing the correct thing .
//	connect(m_currentFrameSpinBox , SIGNAL(valueChanged(double)) , this , SLOT(currentFrameChanged()));
		
	connect(m_startSpinBox , SIGNAL(editingFinished()) , this , SLOT(updateFrameRange()));
	connect(m_endSpinBox , SIGNAL(editingFinished()) , this , SLOT(updateFrameRange()));

	connect( m_goToStartFrameButton , SIGNAL(clicked()) , this , SLOT( goToStartFrame() ) );
	connect( m_goToPreviousFrameButton , SIGNAL(clicked()) , this , SLOT( goToPreviousFrame() )  );
	
	connect( m_playButton , SIGNAL(clicked()) , this , SLOT( play() ) );
	
	connect( m_goToNextFrameButton , SIGNAL(clicked()) , this , SLOT( goToNextFrame() ) );
	connect( m_goToEndFrameButton , SIGNAL(clicked()) , this , SLOT( goToEndFrame() ) );

	connect( m_timer , SIGNAL(timeout()) , this , SLOT(timerUpdate()) );

	connect( m_frameRateComboBox , SIGNAL(activated(int)) , this , SLOT( frameRateChanged(int))  );
	connect( m_loopingComBox , SIGNAL(activated(int)) , this , SLOT( loopingChanged(int))  );
	
}

void TimeSliderWidget::setTime(int time)
{
  if(m_settingTime)
    return;
  m_settingTime = true;

  m_frameSlider->setValue(time);
  m_currentFrameSpinBox->setValue(time);

  emit frameChanged(time);

  m_settingTime = false;
}

int TimeSliderWidget::getTime()
{
	return static_cast<int>( m_currentFrameSpinBox->value() );
}

void TimeSliderWidget::sliderChanged(int frame)
{
	setTime( frame );
}

void TimeSliderWidget::frameChangedBy(int frame)
{
	setTime( static_cast<int>(  m_currentFrameSpinBox->value()+frame ) );
}

void TimeSliderWidget::currentFrameChanged()
{
	setTime( static_cast<int>(  m_currentFrameSpinBox->value() ) );
}

void TimeSliderWidget::setTimeRange(int start , int end)
{
	m_startSpinBox->blockSignals(true);
	m_startSpinBox->setValue( static_cast<int>( start ) );
	m_startSpinBox->blockSignals(false);
	
	m_endSpinBox->blockSignals(true);
	m_endSpinBox->setValue( static_cast<int>( end ) );
	m_endSpinBox->blockSignals(false);
	
	// update the other elements 
	updateFrameRange();

	// current time need to be on the bound
	int currentTime = getTime();
	if (currentTime > end)
		setTime(end);
	
	if (currentTime < start )
		setTime(start);
	
}


void TimeSliderWidget::updateFrameRange()
{
	m_currentFrameSpinBox->setMinimum( m_startSpinBox->value() );
	m_currentFrameSpinBox->setMaximum( m_endSpinBox->value() );
	
	m_frameSlider->setMinimum( static_cast<int>( m_startSpinBox->value() ) );
	m_frameSlider->setMaximum( static_cast<int>( m_endSpinBox->value() ) );
}


void TimeSliderWidget::play()
{
	if (m_timer->isActive() )
	{
		m_timer->stop();
		m_playButton->setText(">");
	}
	else
	{
		m_timer->start();
		m_playButton->setText("||");
	}
}


void TimeSliderWidget::goToStartFrame()
{
	int newFrame = static_cast<int>( m_startSpinBox->value() );
	setTime(newFrame);	
}

void TimeSliderWidget::goToPreviousFrame()
{
	int newFrame = max( getTime()-1 , static_cast<int>( m_startSpinBox->value() ) );
	setTime(newFrame);
}

void TimeSliderWidget::goToNextFrame()
{
	int newFrame = min( getTime()+1 , static_cast<int>( m_endSpinBox->value() ) );
	setTime(newFrame);
}
		
void TimeSliderWidget::goToEndFrame()
{
	int newFrame = static_cast<int>( m_endSpinBox->value() );
	setTime(newFrame);	
}

void TimeSliderWidget::timerUpdate()
{
	int newTime = getTime()+m_direction;
	if ( newTime > m_endSpinBox->value() )
	{
		if ( m_looping == 0 )
			play();
		else if ( m_looping == 1 )
		{
			goToStartFrame();
		}
		else
		{
			m_direction *= -1;
			newTime = getTime()+m_direction;
			setTime( newTime );
		}
	}	
	else if ( newTime < m_startSpinBox->value() ) 
	{
		if ( m_looping )
			play();
		else if (m_looping == 1)
			goToEndFrame();
		else
		{
			m_direction = -1;
			newTime = getTime()+m_direction;
			setTime( newTime );
		}
	}	
	else
	{
		setTime( newTime );
	}			
	
}

void TimeSliderWidget::frameRateChanged(int index)
{
	if (index == 0)
		m_timer->setInterval(0); // max fps
	else if (index == 1)
		m_timer->setInterval(83); // 12 fps
	else if (index == 2)
		m_timer->setInterval(42); // 24 fps
	else if ( index == 3 )
		m_timer->setInterval(21); // 48 fps
	
}

void TimeSliderWidget::loopingChanged(int index)
{
	m_looping = index;
	if (index != 2)
		m_direction = 1;
}
