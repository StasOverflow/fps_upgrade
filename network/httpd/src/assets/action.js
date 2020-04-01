function findByHash(tag, hash)
{
    var items = document.querySelectorAll(tag);
    
    for(var i = 0; i < items.length; i++)
    {
        if(items[i].hash === hash)
        {
            return items[i];
        }
    }
}

function updateStatusTab()
{
    getJSON('/cgi/status.cgi', function(obj)
    {
        for(var key in obj)
        {
            var id = document.getElementById(key);

            if(id !== null)
            {
                id.querySelector('.value').innerHTML = obj[key];
            }
        }
    });
}

function updateSetupTab()
{
    getJSON('/cgi/setup.cgi', function(obj)
    {
        for(var key in obj)
        {
            var params = document.querySelectorAll('#setup [data-param]');
            
            for(var i = 0; i < params.length; i++)
            {
                if(params[i].tagName.toLowerCase() !== 'input') continue;
                
                switch(params[i].getAttribute('type'))
                {
                    case 'text':
                        if(params[i].getAttribute('data-param') === key)
                        {
                            params[i].value = obj[key];    
                        }
                        break;
                    case 'radio':
                        if(params[i].getAttribute('value').toLowerCase() === obj[key].toLowerCase())
                        {
                            params[i].checked = true;
                        }
                        break;
                    default:
                }
            }
        }
    });
}

function updateNetworkTab()
{
    getJSON('/cgi/lan.cgi', function(obj)
    {
        for(var key in obj)
        {
            var params = document.querySelectorAll('#network [data-param]');

            for(var i = 0; i < params.length; i++)
            {
                switch(params[i].tagName.toLowerCase())
                {
                    case 'input':
                        switch(params[i].getAttribute('type'))
                        {
                            case 'text':
                                if(params[i].getAttribute('data-param') === key)
                                {
                                    params[i].value = obj[key];    
                                }
                                break;
                            case 'radio':
                                if(params[i].getAttribute('value').toLowerCase() === obj[key].toLowerCase())
                                {
                                    params[i].checked = true;
                                }
                                break;
                        }
                        break;
                    case 'li':
                        if(params[i].getAttribute('data-param') === key)
                        {
                            params[i].querySelector('.value').innerHTML = obj[key];
                        }
                        break;
                }
            }
        }
    });
}

function updateDebugTab()
{
    getJSON('/cgi/debug.cgi', function(obj)
    {
        for(var key in obj)
        {
            var id = document.getElementById(key);

            if(id !== null)
            {
                id.querySelector('.value').innerHTML = obj[key];
            }
        }
    });
}

function getJSON(url, callback)
{
    return ajax(url, 'GET', function(data) {callback(JSON.parse(data))});
}

function ajax(url, method, callback)
{
    var request = new XMLHttpRequest();

    request.open(method, url, true);

    request.onload = function()
    {
        if(request.status >= 200 && request.status < 400)
        {
          // Success!
          callback(request.responseText);
        }
        else
        {
          // We reached our target server, but it returned an error
        }
    };

    request.onerror = function()
    {
      // There was a connection error of some sort
    };

    request.send();

    return request;
}

function addClass(el, className)
{
    if(el.classList)
        el.classList.add(className);
    else
        el.className += ' ' + className;
}

function removeClass(el, className)
{
    if(el.classList)
        el.classList.remove(className);
    else
        el.className = el.className.replace(new RegExp('(^|\\b)' + className.split(' ').join('|') + '(\\b|$)', 'gi'), ' ');
}

function serializeFormData(el)
{
    var inputs = el.querySelectorAll('input');

    var formData = {};

    for(var i = 0; i < inputs.length; i++)
    {
        switch(inputs[i].type)
        {
            case 'text':
                formData[inputs[i].name] = inputs[i].value;
                break;
            case 'radio':
                if(inputs[i].checked)
                {
                    formData[inputs[i].name] = inputs[i].value;
                }
                break;
        }
    }

    return formData;
}

function onSubmit(e)
{
    var getString = this.action + '?';

    var formData = serializeFormData(this);
    
    for(var key in formData)
    {
        getString += key + '=' + formData[key] + '&';
    }
    
    getString += 'commit';

    console.log(getString);
    
    ajax(getString, 'GET', function(response)
    {
        console.log('response: ' + response);

        var message = document.getElementById('modal-' + response);

        if(message !== null)
        {
            message.style.display = 'block';
            
            setTimeout(function()
            {
                message.style.display = 'none';

                window.onhashchange();
            }, 1000);
        }
    });

    e.preventDefault();
}

window.onload = function()
{
    window.onhashchange();

    var forms = document.querySelectorAll('form');

    for(var i = 0; i < forms.length; i++)
    {
        if(forms[i].addEventListener)
        {
            forms[i].addEventListener("submit", onSubmit, false); // Cool modern browser!
        }
        else if(forms[i].attachEvent)
        {
            forms[i].attachEvent('onsubmit', onSubmit); // IE needs extra cares.
        }
    }
};

window.onhashchange = function()
{
    this.updateInterval;

    tabChange(window.location.hash);

    clearInterval(this.updateInterval);

    switch(window.location.hash)
    {
        case '#status':
            updateStatusTab();
            this.updateInterval = setInterval(updateStatusTab, 1000);
            break;
        case '#setup':
            updateSetupTab();
            break;
        case '#network':
            updateNetworkTab();
            break;
        case '#debug':
            findByHash('a', window.location.hash).parentNode.style.display = '';
            updateDebugTab();
            this.updateInterval = setInterval(updateDebugTab, 1000);
            break;
        default:
            window.location.hash = '#status';
    }
};

function tabChange(tabURL)
{
    var menuItems = document.querySelectorAll(".main-menu li");
    var item = document.querySelector('.main-menu .active');
    var tab;

    if((tabURL === null) || (tabURL === ''))
    {
        tabURL = '#status';
    }

    if(item !== null)
    {
        tab = document.querySelector(item.querySelector('a').hash);

        removeClass(tab, 'active');

        removeClass(item, 'active');
    }

    for(var i = 0; i < menuItems.length; i++)
    {
        item = menuItems[i];

        tab = document.querySelector(menuItems[i].querySelector('a').hash);

        if(item.firstChild.hash === tabURL)
        {
            addClass(item, 'active');

            addClass(tab, 'active');

            break;
        }
    }
}
