(function () {
    var path = window.location.pathname || "";
    var langMatch = path.match(/\/(en|zh-cn)\//);
    var lang = langMatch ? langMatch[1] : "en";

    var isZh = lang === "zh-cn";

    function text(en, zh) {
        return isZh ? zh : en;
    }

    function renderHeader() {
        var container = document.getElementById("manual-header");
        if (!container) {
            return;
        }

        var targetLang = isZh ? "en" : "zh-cn";
        var targetLabel = isZh ? "English" : "简体中文";
        var targetBadge = isZh ? "EN" : "中";
        var targetPath = path.replace(/\/(en|zh-cn)\//, "/" + targetLang + "/");

        container.innerHTML =
            '<header class="site">' +
            '  <div class="container">' +
            '    <span class="brand">WidgetMarkup &mdash; ' + text("User Manual", "用户手册") + '</span>' +
            '    <nav>' +
            '      <a href="index.html">' + text("Contents", "目录") + '</a>' +
            '      <a href="01-getting-started.html">' + text("Get Started", "快速开始") + '</a>' +
            '      <a href="04-python-integration.html">Python</a>' +
                '      <a class="lang-switch-icon" href="' + targetPath + '" title="' + text("Switch to", "切换到") + ' ' + targetLabel + '" aria-label="' + text("Switch to", "切换到") + ' ' + targetLabel + '">' +
                '        <span class="globe" aria-hidden="true"></span>' +
                '        <span class="badge">' + targetBadge + '</span>' +
                '      </a>' +
            '    </nav>' +
            '  </div>' +
            '</header>';
    }

    function renderFooter() {
        var container = document.getElementById("manual-footer");
        if (!container) {
            return;
        }

        container.innerHTML =
            '<footer class="site">' +
            '  <div class="container">' +
            '    <span>WidgetMarkup ' + text("User Manual", "用户手册") + '</span>' +
            '    <span>&copy; 2025 Wu Zhiwei</span>' +
            '  </div>' +
            '</footer>';
    }

    renderHeader();
    renderFooter();
})();
