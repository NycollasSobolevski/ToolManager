require('global-agent/bootstrap')
const express = require('express');
const routes = require('./routes');

process.env.GLOBAL_AGENT_HTTP_PROXY = 'http://son5ct:PASSWORD@10.224.200.26:8080';

const app = express();

app.use(express.urlencoded({ extended: true }));

// Static files
app.use(express.static('public'));
app.use('/ToolManager', express.static('public'));

// EJS
app.set('views', './src/views');
app.set('view engine', 'ejs');

app.use(routes);

// const PORT = 20031;
// const HOST = '10.234.192.30'
const PORT = 3001;
const HOST = 'localhost'

app.listen(PORT, HOST, () => console.log(`Acesse: http://${HOST}:${PORT}/`));