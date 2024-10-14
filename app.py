import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QSystemTrayIcon, QMenu, QAction


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Главное окно")
        self.setGeometry(100, 100, 300, 200)


class AppTrayIcon(QSystemTrayIcon):
    def __init__(self, app):
        super().__init__(app)
        self.setIcon(app.style().standardIcon(QApplication.TrayIcon))
        self.setToolTip("Приложение")
        self.menu = QMenu()
        self.show_action = QAction("Показать главное окно", self)
        self.show_action.triggered.connect(self.show_main_window)
        self.menu.addAction(self.show_action)
        self.exit_action = QAction("Выход из приложения", self)
        self.exit_action.triggered.connect(app.quit)
        self.menu.addAction(self.exit_action)
        self.setContextMenu(self.menu)
        self.main_window = MainWindow()

    def show_main_window(self):
        self.main_window.show()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    tray_icon = AppTrayIcon(app)
    tray_icon.show()
    sys.exit(app.exec_())
