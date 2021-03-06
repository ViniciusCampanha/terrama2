const KEY = 'terrama2.sid';

var express = require('express'),
    path = require('path'),
    bodyParser = require('body-parser'),
    methodOverride = require('method-override'),
    app = express(),
    load = require('express-load'),
    swig = require('swig'),
    passport = require('./config/Passport'),
    session = require('express-session'),
    flash = require('connect-flash'),
    // i18n = require('i18n-2'),
    i18n = require( "i18n" ),
    Application = require("./core/Application"),
    i18nRoutes = require( "i18n-node-angular" );
    server = require('http').Server(app);

app.use(session({ secret: KEY, name: "TerraMA2WebApp", resave: false, saveUninitialized: false }));

app.use(flash());

// Setting internationalization
i18n.configure({
  locales        : [ "en_US", "pt_BR", "fr_FR", "es_ES"],
  directory      : __dirname + "/locales",
  objectNotation : true
});

Application.setCurrentContext(process.argv[2]);

// Get base url from environment and store in Express.
app.locals.BASE_URL = Application.getContextConfig().basePath;

// Set SWIG template engine
var customSwig = new swig.Swig({varControls: ["{[", "]}"]});
app.engine('html', customSwig.renderFile);
app.set('view engine', 'html');

// Set template directories
app.set('views', __dirname + '/views');

// The order of these calls matters!
app.use(i18n.init);
app.use(i18nRoutes.getLocale);

i18nRoutes.configure(app, {"extension": ".json", directory : __dirname + "/locales/"});

// set up the internacionalization middleware
app.use(function(req, res, next) {
  res.locals.errorMessage = req.flash('error');
  next();
});

app.use(bodyParser.json({ limit: "50mb" }));
app.use(bodyParser.urlencoded({ limit: "50mb", extended: true }));
app.use(methodOverride('_method'));

app.use(app.locals.BASE_URL + 'bower_components', express.static('bower_components'));
app.use(app.locals.BASE_URL, express.static(path.join(__dirname, 'public')));

passport.setupPassport(app);

load('controllers')
  .then('routes')
  .into(app);

module.exports = app;
