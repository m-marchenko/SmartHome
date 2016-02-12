using SmartHome.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;

namespace SmartHome.Controllers
{
    public class HomeController : Controller
    {
        private IRootObject _root;
        public HomeController(IRootObject root)
        {
            _root = root;
        }


        public ActionResult MainView()
        {
            return View(_root);
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