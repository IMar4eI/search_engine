#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include "ConverterJSON.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  private slots:
      void openFile();
  void processSearch();

private:
  void setupUI();
  void setupAnimations();

  QWidget *centralWidget;
  QPushButton *openFileButton;
  QPushButton *searchButton;
  QTextEdit *outputViewer;
  QLabel *titleLabel;

  ConverterJSON jsonConverter;
  QPropertyAnimation *animation;
};
