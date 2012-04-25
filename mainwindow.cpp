#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hsvminmax.h"

#include <iostream>
#include <sstream>
#include <QString>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*
     * variable initializations
     */
    m_image = 0;
    m_image_hsv = 0;

    // setting up the imageLabel
    ui->imageLabel->setBackgroundRole(QPalette::Base);
    ui->imageLabel->setAlignment(Qt::AlignTop & Qt::AlignLeft);

    /*
     * connections
     */
    connect(ui->button_load, SIGNAL(clicked()), this, SLOT(onLoad()));
    connect(ui->button_values, SIGNAL(clicked()), this, SLOT(onGetHSV()));
    connect(this, SIGNAL(pathChanged(QString)), this, SLOT(onPathChange(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * slots
 */
void MainWindow::onLoad()
{
    QString path = QFileDialog::getOpenFileName(this, "Choose an image", ".");
    emit pathChanged(path);
}

void MainWindow::onPathChange(QString p)
{
    // updating the pixmap/graphicsscene
    m_pixmap = QPixmap(p);

    // updating the underlying IplImage
    if (m_image != 0)
        cvReleaseImage(&m_image);
    if (m_image_hsv != 0)
        cvReleaseImage(&m_image_hsv);

    ui->imageLabel->setPixmap(m_pixmap);
    this->resize(0, 0);

    m_image = cvLoadImage(p.toStdString().c_str());
    m_image_hsv = cvCreateImage(cvGetSize(m_image), 8, 3);
    cvCvtColor(m_image, m_image_hsv, CV_BGR2HSV);

    // passing the iplImage to the QLabel
    ui->imageLabel->setImage(m_image, m_image_hsv);
}

void MainWindow::onGetHSV()
{
    ui->imageLabel->setValues();
    HSVMinMax minmax = ui->imageLabel->getValues();

    stringstream sstr;

    // hue
    sstr << "hue_min = " << (int) minmax.hue_min << endl;
    sstr << "hue_max = " << (int) minmax.hue_max << endl;

    // saturation
    sstr << "sat_min = " << (int) minmax.sat_min << endl;
    sstr << "sat_max = " << (int) minmax.sat_max << endl;

    // value
    sstr << "val_min = " << (int) minmax.val_min << endl;
    sstr << "val_max = " << (int) minmax.val_max << endl;

    ui->output_text->setPlainText(QString(sstr.str().c_str()));
    ui->output_text->update();
}
