#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "libusb.h"
#include <QDebug>

#define REQ_TYPE_SET 0x21
#define REQ_TYPE_GET 0xA1

#define REPORT_GET_COMMON_SETTINGS          0x01
#define REPORT_SET_COMMON_SETTINGS          0x02
#define REPORT_CALIBRATE_ELECTRICAL_CURVE   0x04
#define REPORT_CALIBRATE_MECHANICAL_CENTER  0x05
#define REPORT_SAVE_ELECTRICAL_CURVE        0x06
#define REPORT_SAVE_SETTINGS                0x07

unsigned char buffer[64];
libusb_device_handle *device;
GlobalSettingsTypeDef globalSettings;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    libusb_init(NULL);
    device = libusb_open_device_with_vid_pid(NULL,1155,22696);
    if(device){
        ui->readButton->setEnabled(true);
        ui->writeButton->setEnabled(true);
        ui->mechanicalCenterSetButton->setEnabled(true);
        on_readButton_clicked();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    libusb_exit(NULL);
}

void MainWindow::on_writeButton_clicked()
{
    globalSettings.EnableConstantViaSine = ui->enableConstantViaSinCheckBox->isChecked();
    globalSettings.UpdateDurationOnEffectReport = ui->updateDurationOnEffectReportCheckBox->isChecked();

    globalSettings .Direction = ui->invertDirection_1->isChecked()?-1:1;
    globalSettings .SteeringRange = ui->steeringRangeSlider_1->value();
    globalSettings .SteeringSmoothing = ui->steeringSmoothingSlider_1->value();
    globalSettings .TorqueGain = ui->torqueGainSlider_1->value();
    globalSettings .TorqueSmoothing = ui->torqueSmoothingSlider_1->value();
    globalSettings .EffectGains[0] = ui->constantViaSineSlider_1->value();
    globalSettings .EffectGains[1] = ui->constantSlider_1->value();
    globalSettings .EffectGains[2] = ui->rampSlider_1->value();
    globalSettings .EffectGains[3] = ui->squareSlider_1->value();
    globalSettings .EffectGains[4] = ui->sineSlider_1->value();
    globalSettings .EffectGains[5] = ui->triangleSlider_1->value();
    globalSettings .EffectGains[6] = ui->sawtoothUpSlider_1->value();
    globalSettings .EffectGains[7] = ui->sawtoothDownSlider_1->value();
    globalSettings .EffectGains[8] = ui->springSlider_1->value();

    globalSettings .MechanicalCenter = ui->mechanicalCenterSpinBox->value();

    memcpy(buffer+2,&globalSettings,sizeof(GlobalSettingsTypeDef));
    buffer[0]=0x1F;
    buffer[1]=REPORT_SET_COMMON_SETTINGS;
    libusb_control_transfer(device,REQ_TYPE_SET,0x09,0x031F,0x00,buffer,0x41,1000);
}



void MainWindow::on_readButton_clicked()
{
    buffer[0]=0x1F;
    buffer[1]=REPORT_GET_COMMON_SETTINGS;
    libusb_control_transfer(device,REQ_TYPE_SET,0x09,0x031F,0x00,buffer,0x41,1000);
    libusb_control_transfer(device,REQ_TYPE_GET,0x01,0x031F,0x00,buffer,0x41,1000);
    memcpy(&globalSettings,buffer+1,sizeof(GlobalSettingsTypeDef));

    ui->enableConstantViaSinCheckBox->setChecked(globalSettings.EnableConstantViaSine);
    ui->updateDurationOnEffectReportCheckBox->setChecked(globalSettings.UpdateDurationOnEffectReport);

    ui->invertDirection_1->setChecked(globalSettings .Direction==-1);
    ui->steeringRangeSlider_1->setValue(globalSettings .SteeringRange);
    ui->steeringSmoothingSlider_1->setValue(globalSettings .SteeringSmoothing);
    ui->torqueGainSlider_1->setValue(globalSettings .TorqueGain);
    ui->torqueSmoothingSlider_1->setValue(globalSettings .TorqueSmoothing);
    ui->constantViaSineSlider_1->setValue(globalSettings .EffectGains[0]);
    ui->constantSlider_1->setValue(globalSettings .EffectGains[1]);
    ui->rampSlider_1->setValue(globalSettings .EffectGains[2]);
    ui->squareSlider_1->setValue(globalSettings .EffectGains[3]);
    ui->sineSlider_1->setValue(globalSettings .EffectGains[4]);
    ui->triangleSlider_1->setValue(globalSettings .EffectGains[5]);
    ui->sawtoothUpSlider_1->setValue(globalSettings .EffectGains[6]);
    ui->sawtoothDownSlider_1->setValue(globalSettings .EffectGains[7]);
    ui->springSlider_1->setValue(globalSettings .EffectGains[8]);

    ui->mechanicalCenterSpinBox->setValue(globalSettings .MechanicalCenter);
}

void MainWindow::on_calibrateElectricalCenterButton_clicked()
{
    buffer[0]=0x1F;
    buffer[1]=REPORT_CALIBRATE_ELECTRICAL_CURVE;
    libusb_control_transfer(device,REQ_TYPE_SET,0x09,0x031F,0x00,buffer,0x41,1000);
}

void MainWindow::on_saveSettingsButton_clicked()
{
    buffer[0]=0x1F;
    buffer[1]=REPORT_SAVE_SETTINGS;
    libusb_control_transfer(device,REQ_TYPE_SET,0x09,0x031F,0x00,buffer,0x41,1000);
}

void MainWindow::on_saveAdvancedSettingsButton_clicked()
{
    buffer[0]=0x1F;
    buffer[1]=REPORT_SAVE_SETTINGS;
    libusb_control_transfer(device,REQ_TYPE_SET,0x09,0x031F,0x00,buffer,0x41,1000);
}

void MainWindow::on_mechanicalCenterSetButton_clicked()
{
    buffer[0]=0x1F;
    buffer[1]=REPORT_CALIBRATE_MECHANICAL_CENTER;
    libusb_control_transfer(device,REQ_TYPE_SET,0x09,0x031F,0x00,buffer,0x41,1000);
    on_readButton_clicked();
}
