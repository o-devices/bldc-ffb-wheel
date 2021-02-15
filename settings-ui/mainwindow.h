#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#define MAX_EFFECTS 9

typedef struct __attribute__((packed)) {
    uint8_t EnableConstantViaSine;           // Enables possibility to handle zero magnitude sine effect as constant force. Used in R3E. Disable for other games
    uint8_t UpdateDurationOnEffectReport;    // Do not stop effect if time elapsed. Update started at on every packet. R3E does not provide proper duration for effects. // TODO: Investigate R3E
    uint16_t SteeringRange;                  // Encoder ticks. Limits manipulator output and soft stops.
    uint8_t SteeringSmoothing;               // 0..100. Output smoothing.
    int16_t TorqueGain;                      // 0..100. Scales total output. Negative values to invert torque direction
    uint8_t TorqueSmoothing;                 // 0..100. Combined torque smoothing
    int16_t EffectGains[MAX_EFFECTS + 1];    // Use negative values to invert individual Common effect torque. 0 to disable. value 100 will scale by 1.
    int8_t Direction;                       // -1/+1. Stands if positive direction of rotation corresponds to increasing encoder values
    uint16_t MechanicalCenter;               // Offset from encoder Z index to mechanical center in encoder ticks.
    uint16_t EncoderCPR;
    uint16_t CalibrationMagnitude;
} GlobalSettingsTypeDef;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_writeButton_clicked();

    void on_readButton_clicked();

    void on_calibrateElectricalCenterButton_clicked();

    void on_saveSettingsButton_clicked();

    void on_saveAdvancedSettingsButton_clicked();

    void on_mechanicalCenterSetButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
