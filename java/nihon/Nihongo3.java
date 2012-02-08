import java.applet.Applet;
import java.awt.*;

public class Nihongo3 extends Applet {

	int nfont;
	Font font[][];
	String str,fontlist[];

	public void init() {
		fontlist = getToolkit().getFontList();
		nfont = fontlist.length;
		font = new Font[nfont][3];
		for (int i=0; i<nfont; i++) {
			font[i][0] = new Font(fontlist[i], Font.PLAIN, 24);
			font[i][1] = new Font(fontlist[i], Font.BOLD, 24);
			font[i][2] = new Font(fontlist[i], Font.ITALIC, 24);
		}

		str = getParameter("name1");
	}

	public void paint(Graphics g) {
		int y = 40;
		for (int i=0; i<nfont; i++) {
			FontMetrics fm = getFontMetrics(font[i][0]);
			int height = fm.getHeight();
			g.setFont(font[i][0]);
			g.drawString(fontlist[i], 10, y);
			g.drawString(str, 160, y);
			g.setFont(font[i][1]);
			g.drawString(str, 280, y);
			g.setFont(font[i][2]);
			g.drawString(str, 400, y);
			y += height;
		}
	}
}
