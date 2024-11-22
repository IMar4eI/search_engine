#include "MainWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), centralWidget(new QWidget(this)) {
    setupUI();
    setupAnimations();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    setWindowTitle("Search Engine GUI");
    resize(800, 600);

    auto *layout = new QVBoxLayout;

    titleLabel = new QLabel("Search Engine", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");

    openFileButton = new QPushButton("Open JSON File", this);
    searchButton = new QPushButton("Search", this);

    outputViewer = new QTextEdit(this);
    outputViewer->setReadOnly(true);

    layout->addWidget(titleLabel);
    layout->addWidget(openFileButton);
    layout->addWidget(searchButton);
    layout->addWidget(outputViewer);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    connect(openFileButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::processSearch);
}

void MainWindow::setupAnimations() {
    animation = new QPropertyAnimation(titleLabel, "geometry");
    animation->setDuration(1000);
    animation->setStartValue(QRect(0, 0, 0, 50));
    animation->setEndValue(QRect(150, 0, 500, 50));
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start();
}

void MainWindow::openFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open JSON File", "", "JSON Files (*.json)");
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Failed to open the file.");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull()) {
        QMessageBox::warning(this, "Error", "Invalid JSON format.");
        return;
    }

    outputViewer->setText(jsonDoc.toJson(QJsonDocument::Indented));
}

void MainWindow::processSearch() {
    try {
        auto requests = jsonConverter.GetRequests();
        QString output = "Search Requests:\n";
        for (const auto &req : requests) {
            output += QString::fromStdString(req) + "\n";
        }
        outputViewer->setText(output);
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}