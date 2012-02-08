/*
 * Eyecon.java v2.0.1
 *	April 7, 1996
 *
 * Copyright (c) 1996 HuskyLabs.
 *
 * code by Matthew Ross Davis
 * mozart@butterfly.net
 *
 * interactivity for the masses
 *
 * Depending on what's given in the tag, this applet will 
 * 	show different images and play different sounds 
 *	according to the status of the mouse.
 *
 * This version is updated from 2.0 to employ showStatus()
 * and uses MediaTracker for preloading the images
 *
 */

/*			here's a sample HTML file tag
	<HTML>
	<HEAD>
	</HEAD>
	<BODY>
	
	<applet code="Eyecon.class" width=40 height=40>
		<param name="img1" value="images/startup.gif">
		<param name="img2" value="images/mouseEnter.gif">
		<param name="img3" value="images/mouseDown.gif">
		<param name="img4" value="images/mouseUp.gif">
		<param name="snd1" value="audio/mouseExit.au">
		<param name="snd2" value="audio/mouseEnter.au">
		<param name="snd3" value="audio/mouseDown.au">
		<param name="snd4" value="audio/mouseUp.au">
		<param name="URL" value="http://butterfly.net/mozart/">
	</applet>
	
	</BODY>
	</HTML>
*/

import java.awt.*;
import java.net.URL;
import java.net.MalformedURLException;
import java.applet.AudioClip;

public class Eyecon extends java.applet.Applet {

	public String getAppletInfo() {
		return "Eyecon - interactive icon applet, by Matthew Ross Davis";
	}

	public String[][] getParameterInfo() {
		String[][] info = {
			{"img1","image","startup/mouseExit image"},
			{"img1","image","mouseEnter image"},
			{"img1","image","mouseDown image"},
			{"img1","image","mouseUp image"},
			{"snd1","sound","mouseExit sound"},
			{"snd2","sound","mouseEnter sound"},
			{"snd3","sound","mouseDown sound"},
			{"snd4","sound","mouseUp sound"},
			{"URL","url","linked URL"},
		};
		return info;
	}
	
	// audio objects
	AudioClip sound1 = null;								
	AudioClip sound2 = null;
	AudioClip sound3 = null;
	AudioClip sound4 = null;
	
	// image objects
	Image im1 = null;										
	Image im2 = null;
	Image im3 = null;
	Image im4 = null;
	
	// current image for drawing
	Image current = null;
	
	// MediaTracker object for loading images
	protected MediaTracker trackimgs;

	// image and sound filename variables
	String imgFile1, imgFile2, imgFile3, imgFile4;			
	String soundFile1, soundFile2, soundFile3, soundFile4;
	
	// URL variable and object
	String iconURL;
	Linkme link;

	public void init() {
	
		// assign sound and image filename variables
		soundFile1 = getParameter("snd1");
		soundFile2 = getParameter("snd2");
		soundFile3 = getParameter("snd3");
		soundFile4 = getParameter("snd4");
		imgFile1 = getParameter("img1");
		imgFile2 = getParameter("img2");
		imgFile3 = getParameter("img3");
		imgFile4 = getParameter("img4");
		
		// the URL and an object to deal with it
		iconURL = getParameter("URL");
		link = new Linkme(iconURL);

		// load the sound files
		sound1 = getAudioClip(getCodeBase(), soundFile1);
		sound2 = getAudioClip(getCodeBase(), soundFile2);
		sound3 = getAudioClip(getCodeBase(), soundFile3);
		sound4 = getAudioClip(getCodeBase(), soundFile4);
		
		// create a new MediaTracker object
		trackimgs = new MediaTracker(this);
		
		// load the graphics into the objects
		// and put each reference into the MediaTracker
		im1 = getImage(getCodeBase(), imgFile1);									
		trackimgs.addImage(im1, 1);							
		im2 = getImage(getCodeBase(), imgFile2);
		trackimgs.addImage(im2, 2);
		im3 = getImage(getCodeBase(), imgFile3);
		trackimgs.addImage(im3, 3);
		im4 = getImage(getCodeBase(), imgFile4);
		trackimgs.addImage(im4, 4);

		// the following four blocks of code check to see if an
		// image exists, then MediaTracker loads the image
		if ( im1 != null) {
			showStatus("Image 1 is loading");
			try { trackimgs.waitForID(1); }
			catch (InterruptedException e) {}
			if (trackimgs.isErrorID(1)) {
				showStatus("Error with Image 1. Aborting applet.");
				return;
			}
		}
		
		if ( im2 != null ) {
			showStatus("Image 2 is loading");
			try { trackimgs.waitForID(2); }
			catch (InterruptedException e) {}
			if (trackimgs.isErrorID(2)) {
				showStatus("Error with Image 2. Aborting applet.");
				return;
			}
		}
		
		if ( im3 != null ) {
			showStatus("Image 3 is loading");
			try { trackimgs.waitForID(3); }
			catch (InterruptedException e) {}
			if (trackimgs.isErrorID(3)) {
				showStatus("Error with Image 3. Aborting applet.");
				return;
			}
		}
		
		if ( im4 != null ) {
			showStatus("Image 4 is loading");
			try { trackimgs.waitForID(4); }
			catch (InterruptedException e) {}
			if (trackimgs.isErrorID(4)) {
				showStatus("Error with Image 4. Aborting applet.");
				return;
			}
		}
		
		showStatus("Image loading complete. Applet ready.");
				
		
		// use the mouseEnter() image (im1) on startup
		// and resize the applet to image dimensions
		if (im1 != null) {
			current = im1;
			resize(im1.getWidth(null), im1.getHeight(null));
		}
		repaint();
	}

	// on a mouse enter, show the second
	// and resize the applet to image dimensions
	public boolean mouseEnter(java.awt.Event event, int x, int y) {
		if (im2 != null) {
			current = im2;
			resize(im2.getWidth(null), im2.getHeight(null));
			repaint();
			if (sound2 != null)
				sound2.play();
		}
		// show the target URL
		showStatus(iconURL);									
		return true;
	}

	// on a mouse exit, show the first
	// and resize the applet to image dimensions
	public boolean mouseExit(java.awt.Event event, int x, int y) {
		if (im1 != null) {
			current = im1;									
			resize(im1.getWidth(null), im1.getHeight(null));
			repaint();
			if (sound1 != null)
				sound1.play();
		}
		// clear the target URL
		showStatus("");											
		return true;
	}

	// on a mouse down, show the third
	// and resize the applet to image dimensions
	public boolean mouseDown(java.awt.Event event, int x, int y) {
		if (im3 != null) {
			current = im3;
			resize(im3.getWidth(null), im3.getHeight(null));
			repaint();
			if (sound3 != null)
				sound3.play();
		}
		return true;
	}

	// on a mouse up, show the fourth
	// and resize the applet to image dimensions
	public boolean mouseUp(java.awt.Event event, int x, int y) {
		if (im4 != null) {
			current = im4;		
			resize(im4.getWidth(null), im4.getHeight(null));
			repaint();
			if (sound4 != null)
				sound4.play();
		}
		// link to the URL
		getAppletContext().showDocument(link.url);				
		return true;
	}

	// draw the current image
	public void paint(Graphics g) {
		g.drawImage(current, 0, 0, this);	
	}

	// prevent clearing of images - this causes
	// the images to be written over each other,
	// reducing flicker when they change
	public void update(Graphics g) {
		paint(g);
	}
}


// class for url linking
class Linkme {													
	URL url;
	Linkme(String theURL) {
		try { this.url = new URL(theURL); }
		catch ( MalformedURLException e) {
			System.out.println("Bad URL: " + theURL);
		}
	}
}
