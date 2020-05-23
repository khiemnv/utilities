using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using CefSharp;
using CefSharp.Event;
using CefSharp.SchemeHandler;

namespace browser_javascript_binding
{
    public partial class Form1 : Form
    {
        protected CefSharp.WinForms.ChromiumWebBrowser browser;
       
        public Form1()
        {
            InitializeComponent();

            browser = new CefSharp.WinForms.ChromiumWebBrowser("");
            browser.Dock = DockStyle.Fill;
            this.Controls.Add(browser);

            var file = @"D:\tmp\github\utilities\KinhPhat\browser_javascript_binding\simple.html";
            browser.JavascriptObjectRepository.Register("jsHandler", jsHandler, true);
            browser.Load(file);
        }

        JsHandler jsHandler = new JsHandler();

        private void onmyevent(object sender, JavascriptBindingCompleteEventArgs e)
        {
           
            Debug.WriteLine(e);
        }
        EventHandler<JavascriptBindingCompleteEventArgs>  eventHandler;
        class JsHandler
        {
            public void HandleJsCall(int arg)
            {
                MessageBox.Show($"Value Provided From JavaScript: {arg.ToString()}", "C# Method Called");
            }
        }
    }
}
