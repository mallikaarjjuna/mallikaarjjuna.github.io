<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
                xmlns:html="http://www.w3.org/TR/REC-html40"
                xmlns:sitemap="http://www.sitemaps.org/schemas/sitemap/0.9"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" version="1.0" encoding="UTF-8" indent="yes"/>
  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
        <title>XML Sitemap</title>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <style type="text/css">
          body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #121212; color: #e0e0e0; padding: 30px; }
          h1 { color: #f39c12; }
          table { width: 100%; border-collapse: collapse; margin-top: 20px; background: #1e1e24; }
          th, td { padding: 10px; text-align: left; border: 1px solid #333; }
          th { background-color: #2a2a35; color: #f39c12; }
          a { color: #4af626; text-decoration: none; }
          a:hover { text-decoration: underline; }
        </style>
      </head>
      <body>
        <h1>Jyotisha Engine Sitemap</h1>
        <p>This is a styled XML sitemap designed for search engines like Google.</p>
        <table>
          <tr>
            <th>URL</th>
            <th>Priority</th>
            <th>Change Frequency</th>
          </tr>
          <xsl:for-each select="sitemap:urlset/sitemap:url">
            <tr>
              <td>
                <xsl:variable name="itemURL">
                  <xsl:value-of select="sitemap:loc"/>
                </xsl:variable>
                <a href="{$itemURL}">
                  <xsl:value-of select="sitemap:loc"/>
                </a>
              </td>
              <td><xsl:value-of select="sitemap:priority"/></td>
              <td><xsl:value-of select="sitemap:changefreq"/></td>
            </tr>
          </xsl:for-each>
        </table>
      </body>
    </html>
  </xsl:template>
</xsl:stylesheet>