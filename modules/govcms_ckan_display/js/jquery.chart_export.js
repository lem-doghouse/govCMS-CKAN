(function ($) {

  "use strict";

  /*
   * ChartExport jQuery plugin.
   * ---------------------------
   * This plugin handles exporting a chart to a svg/png format.
   *
   * Usage.
   * ------
   * Bind this to the button you wish to trigger the download of the chart.
   * Eg. $('button).chartExport({svg: 'svg.my-chart', format: 'svg'});
   *
   * Options/settings:
   * - svg: (required) This is either a jQuery object or jQuery selector for the chart.
   * - format: (string) The save as format (svg or png). Defaults to svg
   * - filename: (string) The filename to use when saving, Defaults to 'chart'
   * - includeC3jsStyles: (bool) Should we inject some c3js styles that fix output bugs. Defaults to true
   * - errorMsg: (string) The error message to display if no browser support for downloading.
   */

  /**
   * chartExport class.
   *
   * @param dom
   *   The dom for the triggering element (eg. the button/link).
   * @param settings
   *   Settings for the class. @see defaults.
   * @returns {chartExport}
   *   Return self for chaining.
   */
  var ChartExport = function (dom, settings) {
    var self = this;

    // Defaults.
    self.defaults = {
      // The button this is getting bound to.
      $button: $(dom),
      // Chart SVG selector or jQuery object.
      svg: 'svg',
      // Format to save as.
      format: 'svg',
      // Filename (without extension).
      filename: 'chart',
      // Include c3js styles (fixes display bugs with c3js charts)
      includeC3jsStyles: true,
      c3jsStyles: 'svg{font:10px sans-serif}line,path{fill:none;stroke:#000}',
      // Passed Validation requirements.
      valid: true,
      // Error message.
      errorMsg: 'Sorry, your browser does not support this function.'
    };

    // Update defaults with passed settings.
    self.settings = $.extend(self.defaults, settings);

    /*
     * Validate requirements.
     */
    self.validateRequirements = function () {
      if (self.settings.format === 'svg') {
        // This *should cover html5 requirements too. TODO confirm that is enough?
        self.settings.valid = (typeof btoa === 'function');
      } else {
        // Assuming nothing else is using Blob.
        self.settings.valid = (typeof Blob === 'function');
      }

      // Return self for chaining.
      return self;
    };

    /*
     * Parse the svg object.
     */
    self.parseSvg = function () {
      // Processed class.
      var processedClass = 'chart-export-processed';

      // If svg is not a jQuery object, make it one.
      if (!self.settings.svg instanceof jQuery) {
        self.settings.svg = $(self.settings.svg);
      }

      // Ensure our svg is actually an svg (not a wrapper element).
      self.settings.svg = self.settings.svg.is('svg') ? self.settings.svg : self.settings.svg.find('> svg');

      // If no svg, validation failed.
      if (self.settings.svg.length === 0 || self.settings.svg.hasClass(processedClass)) {
        self.settings.valid = false;
        return;
      }

      // Add some XML attributes.
      self.settings.svg
        .attr('version', 1.1)
        .attr('xmlns', 'http://www.w3.org/2000/svg');

      // Include c3js styles.
      self.includeC3jsStyle();

      // Prevent duplicate parsing.
      self.settings.svg.addClass(processedClass);

      // Return self for chaining.
      return self;
    };

    /*
     * Inject c3js styles if required.
     */
    self.includeC3jsStyle = function () {
      // Check if styles need to be added first.
      if (!self.settings.includeC3jsStyles) {
        return;
      }

      // Create a style element.
      self.$c3styles = $('<style>')
        .attr('type', 'text/css')
        .html("<![CDATA[\n" + self.settings.c3jsStyles + "\n]]>")
        .appendTo($('defs', self.settings.svg));

      // Return self for chaining.
      return self;
    };

    /*
     * Save as SVG.
     */
    self.saveSVG = function () {
      // Get the html and return as a blob using FileSaver.
      var src = 'data:image/svg+xml;base64,' + btoa(self.svgHtml);
      self.downloadHtml5(src);
    };

    /*
     * Save as PNG.
     */
    self.savePNG = function () {
      // Create a new image and add the svg as a src.
      var image = new Image();
      image.src = 'data:image/svg+xml;base64,' + btoa(self.svgHtml);

      // On image load, trigger its download with html5 download attr.
      image.onload = function () {
        // Once loaded, turn the image object into a 2d canvas.
        var canvas = document.createElement('canvas'), context;
        canvas.width = image.width;
        canvas.height = image.height;
        context = canvas.getContext('2d');
        context.drawImage(image, 0, 0);

        // Trigger a file download.
        self.downloadHtml5(canvas.toDataURL('image/png'));
      };
    };

    /*
     * Trigger a download using the html5 download attribute.
     */
    self.downloadHtml5 = function (href) {
      // This doesn't work if we use jQuery to create the el so vanilla JS it is.
      var a = document.createElement('a');
      // Define the filename in the 'save as' box (also triggers the save as box)
      a.download = self.settings.filename + '.' + self.settings.format;
      // Add the contents of what we are saving.
      a.href = href;
      // Needs to be in the dom to trigger a click.
      document.body.appendChild(a);
      a.click();
      // Cleanup afterwards. TODO: Test lots to ensure doesn't affect download.
      a.remove();
    };

    /*
     * Click action.
     */
    self.bindClick = function (e) {
      e.preventDefault();

      // If not passed validation, clicking the button returns an error msg.
      if (self.settings.valid === false) {
        return alert(self.settings.errorMsg);
      }

      // Get the html for the svg.
      self.svgHtml = self.settings.svg[0].outerHTML;

      // Call the save method based on format.
      switch (self.settings.format.toLowerCase()) {
        case 'svg':
          self.saveSVG();
          break;
        case 'png':
          self.savePNG();
          break;
      }
    };

    /*
     * Init the class.
     */
    self.init = function () {
      // Only one export per button.
      if (self.settings.$button.hasClass('chart-export-processed')) {
        return;
      }

      // Prepare requirements and svg.
      self
        .validateRequirements()
        .parseSvg();

      // Bind click and mark as processed.
      self.settings.$button
        .click(self.bindClick)
        .addClass('chart-export-processed');
    };

    // Kick it off on constuct.
    self.init();

    // Return self for chaining.
    return self;
  };


  /**
   * jQuery plugin/function.
   */
  $.fn.chartExport = function (settings) {
    return this.each(function (i, dom) {
      new ChartExport(dom, settings)
    });
  };

})(jQuery);
