<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/transitional.dtd">
<html>
<head>

  <meta http-equiv="content-type" content="text/html; charset=utf-8">
  <title>Source file src/pkg/exp/draw/event.go</title>

  <link rel="stylesheet" type="text/css" href="/doc/style.css">
  <script type="text/javascript" src="/doc/godocs.js"></script>

</head>

<body>

  <script>
    // Catch 'enter' key down events and trigger the search form submission.
    function codesearchKeyDown(event) {
      if (event.which == 13) {
        var form = document.getElementById('codesearch');
        var query = document.getElementById('codesearchQuery');
        form.q.value = "lang:go package:go.googlecode.com " + query.value;
        document.getElementById('codesearch').submit();
}      return true;
}
    // Capture the submission event and construct the query parameter.
    function codeSearchSubmit() {
      var query = document.getElementById('codesearchQuery');
      var form = document.getElementById('codesearch');
      form.q.value = "lang:go package:go.googlecode.com " + query.value;
      return true;
}  </script>

<div id="topnav">
  <table summary="">
    <tr>
      <td id="headerImage">
        <a href="/"><img src="/doc/logo-153x55.png" height="55" width="153" alt="Go Home Page" style="border:0" /></a>
      </td>
      <td>
        <div id="headerDocSetTitle">The Go Programming Language</div>
      </td>
      <td>
        <!-- <table>
          <tr>
            <td>
              <! The input box is outside of the form because we want to add
              a couple of restricts to the query before submitting. If we just
              add the restricts to the text box before submitting, then they
              appear in the box when the user presses 'back'. Thus we use a
              hidden field in the form. However, there's no way to stop the
              non-hidden text box from also submitting a value unless we move
              it outside of the form
              <input type="search" id="codesearchQuery" value="" size="30" onkeydown="return codesearchKeyDown(event);"/>
              <form method="GET" action="http://www.google.com/codesearch" id="codesearch" class="search" onsubmit="return codeSearchSubmit();" style="display:inline;">
                <input type="hidden" name="q" value=""/>
                <input type="submit" value="Code search" />
                <span style="color: red">(TODO: remove for now?)</span>
              </form>
            </td>
          </tr>
          <tr>
            <td>
              <span style="color: gray;">(e.g. &ldquo;pem&rdquo; or &ldquo;xml&rdquo;)</span>
            </td>
          </tr>
        </table> -->
      </td>
    </tr>
  </table>
</div>

<div id="linkList">
  <ul>
    <li class="navhead"><a href="/">Home</a></li>
    <li class="blank">&nbsp;</li>

    <li class="navhead"><a href="/doc/install.html">Installing Go</a></li>
    <li class="navhead"><a href="/doc/go_learning.html">Go Resources</a></li>
    <li class="blank">&nbsp;</li>

    <li class="navhead">Selected Documents</li>
    <li><a href="/doc/go_tutorial.html">Tutorial</a></li>
    <li><a href="/doc/effective_go.html">Effective Go</a></li>
    <li><a href="/doc/go_faq.html">FAQ</a></li>
    <li><a href="/doc/go_lang_faq.html">Language Design FAQ</a></li>
    <li><a href="/doc/go_programming_faq.html">Programming FAQ</a></li>

    <li class="blank">&nbsp;</li>
    <li class="navhead">References</li>
    <li><a href="/doc/go_spec.html">Language Specification</a></li>
    <li><a href="/pkg">Package documentation</a></li>
    <li><a href="/cmd">Command documentation</a></li>
    <li><a href="/src">Source files</a></li>

    <li class="blank">&nbsp;</li>
    <li class="navhead">Help &amp; Community</li>
    <li><a href="http://blog.golang.org/">Go Blog</a></li>
    <li><a href="http://groups.google.com/group/golang-nuts">Go Nuts mailing list</a></li>
    <li>#go-nuts on irc.freenode.net</li>
    <li><a href="http://twitter.com/go_nuts">@go_nuts on Twitter</a></li>
    <li><a href="http://youtube.com/user/gocoding">gocoding YouTube Channel</a></li>
    <li><a href="http://code.google.com/p/go/issues/list">Issue tracker</a></li>
    <li><a href="http://code.google.com/p/go/wiki/WikiIndex">Go Wiki</a></li>

    <li class="blank">&nbsp;</li>
    <li class="navhead">Go Dashboard</li>
    <li><a href="http://godashboard.appspot.com/">Build Status</a></li>
    <li><a href="http://godashboard.appspot.com/package">External Packages</a></li>
    <li><a href="http://godashboard.appspot.com/benchmarks">Benchmarks</a></li>

    <li class="blank">&nbsp;</li>
    <li class="blank">&nbsp;</li>
    <li class="navhead">Go code search</li>
    <form method="GET" action="/search" class="search">
    <input type="search" name="q" value="" size="25" style="width:80%; max-width:200px" />
    <input type="submit" value="Go" />
    </form>

    <li class="blank">&nbsp;</li>
    <li class="navhead">Last update</li>
	<li>Fri Sep 10 01:40:37 PDT 2010</li>
	<li>release.2010-09-06</li>
  </ul>
</div>

<div id="content">
  <!-- Menu is HTML-escaped elsewhere -->

    <h1 id="generatedHeader">Source file src/pkg/exp/draw/event.go</h1>

  <p>
  <!-- The Table of Contents is automatically inserted in this <div>.
       Do not delete this <div>. -->
  <div id="nav"></div>
  </p>

  <!-- Content is HTML-escaped elsewhere -->
  <!--
	Copyright 2009 The Go Authors. All rights reserved.
	Use of this source code is governed by a BSD-style
	license that can be found in the LICENSE file.
-->

<script src="http://www.google.com/jsapi"></script>
<script src="/doc/popups.js"></script>
<script>
var popup_data = [
]


google.load("jquery", "1");
google.setOnLoadCallback(function() {
        godocs_bindPopups(popup_data);
});
</script>

<pre><a id="L1"></a><span class="comment">// Copyright 2009 The Go Authors.  All rights reserved.</span>
<a id="L2"></a><span class="comment">// Use of this source code is governed by a BSD-style</span>
<a id="L3"></a><span class="comment">// license that can be found in the LICENSE file.</span>

<a id="L5"></a>package draw

<a id="L7"></a>import (
    <a id="L8"></a>&#34;image&#34;
<a id="L9"></a>)

<a id="L11"></a><span class="comment">// A Context represents a single graphics window.</span>
<a id="L12"></a>type Context interface {
    <a id="L13"></a><span class="comment">// Screen returns an editable Image of window.</span>
    <a id="L14"></a>Screen() Image

    <a id="L16"></a><span class="comment">// FlushImage flushes changes made to Screen() back to screen.</span>
    <a id="L17"></a>FlushImage()

    <a id="L19"></a><span class="comment">// EventChan returns a channel carrying UI events such as key presses,</span>
    <a id="L20"></a><span class="comment">// mouse movements and window resizes.</span>
    <a id="L21"></a>EventChan() &lt;-chan interface{}
<a id="L22"></a>}

<a id="L24"></a><span class="comment">// A KeyEvent is sent for a key press or release.</span>
<a id="L25"></a>type KeyEvent struct {
    <a id="L26"></a><span class="comment">// The value k represents key k being pressed.</span>
    <a id="L27"></a><span class="comment">// The value -k represents key k being released.</span>
    <a id="L28"></a><span class="comment">// The specific set of key values is not specified,</span>
    <a id="L29"></a><span class="comment">// but ordinary characters represent themselves.</span>
    <a id="L30"></a>Key int
<a id="L31"></a>}

<a id="L33"></a><span class="comment">// A MouseEvent is sent for a button press or release or for a mouse movement.</span>
<a id="L34"></a>type MouseEvent struct {
    <a id="L35"></a><span class="comment">// Buttons is a bit mask of buttons: 1&lt;&lt;0 is left, 1&lt;&lt;1 middle, 1&lt;&lt;2 right.</span>
    <a id="L36"></a><span class="comment">// It represents button state and not necessarily the state delta: bit 0</span>
    <a id="L37"></a><span class="comment">// being on means that the left mouse button is down, but does not imply</span>
    <a id="L38"></a><span class="comment">// that the same button was up in the previous MouseEvent.</span>
    <a id="L39"></a>Buttons int
    <a id="L40"></a><span class="comment">// Loc is the location of the cursor.</span>
    <a id="L41"></a>Loc image.Point
<a id="L42"></a>}

<a id="L44"></a><span class="comment">// A ConfigEvent is sent each time the window&#39;s color model or size changes.</span>
<a id="L45"></a><span class="comment">// The client should respond by calling Context.Screen to obtain a new image.</span>
<a id="L46"></a>type ConfigEvent struct {
    <a id="L47"></a>Config image.Config
<a id="L48"></a>}
</pre>

</div>

<div id="footer">
<p>Except as noted, this content is
   licensed under <a href="http://creativecommons.org/licenses/by/3.0/">
   Creative Commons Attribution 3.0</a>.
</div>

<script type="text/javascript">
var gaJsHost = (("https:" == document.location.protocol) ? "https://ssl." : "http://www.");
document.write(unescape("%3Cscript src='" + gaJsHost + "google-analytics.com/ga.js' type='text/javascript'%3E%3C/script%3E"));
</script>
<script type="text/javascript">
var pageTracker = _gat._getTracker("UA-11222381-2");
pageTracker._trackPageview();
</script>
</body>
</html>
<!-- generated at Tue Sep  7 16:46:01 EST 2010 -->
