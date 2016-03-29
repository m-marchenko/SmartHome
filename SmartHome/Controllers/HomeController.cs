using SmartHome.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using System.Xml.Serialization;

namespace SmartHome.Controllers
{
    public class HomeController : Controller
    {
        private IRootObject _root;
        public HomeController(IRootObject root)
        {
            _root = root;

            //XmlSerializer xs = new XmlSerializer(typeof(Fazenda));

            //StringWriter sw = new StringWriter();

            //xs.Serialize(sw, root);

            //var s = sw.ToString();
        }


        public ActionResult MainView()
        {
            return View(_root);
        }

        public ActionResult Refresh(string id)
        {
            var m = _root.FindCompositeObject(id);

            return PartialView("CompositeObject", m);
        }

        public ActionResult Index()
        {
            return View();
        }

        public ActionResult About()
        {
            ViewBag.Message = "Your application description page.";

            return View();
        }

        public ActionResult Contact()
        {
            ViewBag.Message = "Your contact page.";

            return View();
        }
    }
}