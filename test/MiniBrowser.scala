import java.awt._
import java.awt.event._
import java.net._
import java.util._
import javax.swing._
import javax.swing.event._
import javax.swing.text.html._
import MiniBrowser._
//remove if not needed
import scala.collection.JavaConversions._
/**
 * Web browser based on java tutorial:
 * http://www.java-tips.org/java-se-tips-100019/15-javax-swing/1394-how-to-create-a-simple-browser-in-swing.html
 *
 */


object MiniBrowser {

  def main(args: Array[String]) {
    var browser = new MiniBrowser()
    browser.show()
  }
}

class MiniBrowser extends JFrame("Mini Browser") with HyperlinkListener {

  // Default window configurations:
  setSize(640, 480)
  setLocationRelativeTo(null)
  addWindowListener(new WindowAdapter() {
    override def windowClosing(e: WindowEvent) {
      actionExit()
    }
  })

  private var locationTextField: JTextField = new JTextField(35)
  private var displayEditorPane: JEditorPane = new JEditorPane()

  // creating menus:
  var menu = new JMenuBar()
  var fileMenu = new JMenu("File")
  fileMenu.setMnemonic(KeyEvent.VK_F)
  var fileExitMenuItem = new JMenuItem("Exit", KeyEvent.VK_X)
  fileExitMenuItem.addActionListener(new ActionListener() {
    def actionPerformed(e: ActionEvent) {
      actionExit()
    }
  })
  fileMenu.add(fileExitMenuItem)
  menu.add(fileMenu)
  setJMenuBar(menu)

  // Making a buttonPanel with locationTextField and a "GO!" button:
  var buttonPanel = new JPanel()
  locationTextField.addKeyListener(new KeyAdapter() {
    override def keyReleased(e: KeyEvent) {
      if (e.getKeyCode == KeyEvent.VK_ENTER) {
        actionGo()
      }
    }
  })
  buttonPanel.add(locationTextField)
  var goButton = new JButton("GO")
  goButton.addActionListener(new ActionListener() {
    def actionPerformed(e: ActionEvent) {
      actionGo()
    }
  })
  buttonPanel.add(goButton)

  // setting displayEditorPane configurations:
  displayEditorPane.setContentType("text/html")
  displayEditorPane.setEditable(false)
  displayEditorPane.addHyperlinkListener(this)
  getContentPane.setLayout(new BorderLayout())
  getContentPane.add(buttonPanel, BorderLayout.NORTH)
  getContentPane.add(new JScrollPane(displayEditorPane), BorderLayout.CENTER)

  private def actionExit() {
    System.exit(0)
  }

  private def actionGo() {
    var verifiedUrl = verifyUrl(locationTextField.getText)
    if (verifiedUrl != null) {
      showPage(verifiedUrl)
    } else {
      showError("Invarid URL")
    }
  }

  private def showError(errorMessage: String) {
    JOptionPane.showMessageDialog(this, errorMessage, "Error", JOptionPane.ERROR_MESSAGE)
  }

  private def verifyUrl(url: String): URL = {
    if (!url.toLowerCase().startsWith("http://")) return null
    var verifiedUrl: URL = null
    try {
      verifiedUrl = new URL(url)
    } catch {
      case e: Exception => return null
    }
    verifiedUrl
  }

  // ------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------
  // CHANGE THESE FUNCTION TO WORK WITH APPLICATION CLIENT SCRIPT!!!!
  // ------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------------------

  private def showPage(pageUrl: URL) {
    setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR))
    try {
      var currentUrl = displayEditorPane.getPage
      displayEditorPane.setPage(pageUrl)
      var newUrl = displayEditorPane.getPage
      locationTextField.setText(newUrl.toString)
      updateButtons()
    } catch {
      case e: Exception => {}
    } finally {
      setCursor(Cursor.getDefaultCursor)
    }
  }

  private def updateButtons() {
    var currentUrl = displayEditorPane.getPage
  }

  def hyperlinkUpdate(event: HyperlinkEvent) {
    var eventType = event.getEventType
    if (eventType == HyperlinkEvent.EventType.ACTIVATED) {
      if (event.isInstanceOf[HTMLFrameHyperlinkEvent]) {
        var linkEvent = event.asInstanceOf[HTMLFrameHyperlinkEvent]
        var document = displayEditorPane.getDocument.asInstanceOf[HTMLDocument]
        document.processHTMLFrameHyperlinkEvent(linkEvent)
      } else {
        showPage(event.getURL)
      }
    }
  }
}