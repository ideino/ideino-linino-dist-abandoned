var file_name = "", paths = "", my_editor = null ;

window.onload = loadFile();		

tinymce.init({
	selector: "div.editable",
	inline: true,
	plugins: [
		"save",
		"advlist autolink lists link image charmap print preview anchor",
		"searchreplace visualblocks code fullscreen",
		"insertdatetime media table contextmenu paste"
	],
	toolbar: " save | insertfile undo redo | styleselect | bold italic | alignleft aligncenter alignright alignjustify | bullist numlist outdent indent | link image",
	save_enablewhendirty: true,
	save_onsavecallback: exportPage,
	fixed_toolbar_container : "#toolbar_div",
	
	setup: function (editor){
		my_editor = editor;
	
		editor.addMenuItem('Insert Library',{
			text:'Insert Library',
			context:'insert',
			image : "/img/icons/libs32.ico",
			onclick:function()
			{
				tinyMCE.activeEditor.windowManager.open({file:'/html/modal/libs_modal.html'});
			}
		}),
		
		editor.addMenuItem('Insert Button',{
			text:'Insert Button',
			context:'insert',
			image : "/img/icons/button32.ico",
			onclick:function()
			{
				tinyMCE.activeEditor.windowManager.open({file:'/html/modal/button_modal.html'});
				
			}
		}),
		
		editor.addMenuItem('Insert Text Box',{
			text:'Insert Text Box',
			context:'insert',
			image : "/img/icons/text32.ico",
			onclick:function()
			{
				tinyMCE.activeEditor.windowManager.open({file:'/html/modal/input_modal.html'});
				
			}
		}),
		
		editor.addMenuItem('Insert TextArea',{
			text:'Insert TextArea',
			context:'insert',
			image : "/img/icons/textarea32.ico",
			onclick:function()
			{
				tinyMCE.activeEditor.windowManager.open({file:'/html/modal/text_area_modal.html'});
				
			}
		}),
		
		editor.addMenuItem('Insert Radio Button',{
			text:'Insert Radio Button',
			context:'insert',
			image : "/img/icons/radio32.ico",
			onclick:function()
			{
				tinyMCE.activeEditor.windowManager.open({file:'/html/modal/radio_button_modal.html'});
				
			}
		}),
		
		editor.addMenuItem('Insert Checkbox',{
			text:'Insert Checkbox',
			context:'insert',
			image : "/img/icons/check32.ico",
			onclick:function()
			{
				tinyMCE.activeEditor.windowManager.open({file:'/html/modal/checkbox_modal.html'});
				
			}
		}),
		
		editor.addMenuItem('',{
			text:'-',
			context:'insert',
		})
	}
});
					
function exportPage(){
	var page1 = tinyMCE.activeEditor.getContent().toString();
	//var libs="";
	page1 = page1.split("<p>&nbsp;</p>").join("<br></br>");
				   
	var filename = window.name; 
	if(filename!="undefined" && filename!="")
	{
		var html2save = "<html>"+"\n"+"<head>"+"\n"+"</head>"+"\n"+"<body>"+"\n"+tinyMCE.activeEditor.getBody().outerHTML+"\n"+"</body>"+"\n"+"</html>";
		var page = html2save.replace(/contenteditable="true"/gi,'contenteditable="false"').replace(/>/gi,'>'+'\n');
		
		$.post('/fs/file', {
			_method: 'put',
			path: paths,
			content: page
		}, function(err) {
			if(err) throw err;
		});
	}
	else
		alert("Inserire il nome del file!"); 	
}
			
function openFile(){
	var filename = window.name;
	var arg1 = ["/opt/ideino-linino/ideino/projects/user/examples","html",filename]; 
	var arg2 = encodeURIComponent(JSON.stringify(paths))
	$.get('/fs/file?path=' + arg2, function(data) {
		tinyMCE.activeEditor.setContent(data.replace(/contenteditable="false"/gi,'contenteditable="true"'));
	});
};

function loadFile(){
	if(tinyMCE.activeEditor) { 
		openFile(); 
	} else { 
		setTimeout(loadFile, 500); 
	}
};

function insert_button(id,text,action){
	my_editor.selection.setContent("<button type='button' id='"+id+"' onclick='"+action+"'>"+text+"</button>");
}

function insert_lib(path){
	my_editor.selection.setContent("<script type='text/javascript' src='"+path+"'></script>");
}

function insert_input(id){
	my_editor.selection.setContent("<input type='text' id='"+id+"'></input>");
}

function insert_textarea(id,rows,cols,text){
	my_editor.selection.setContent("<textarea id='"+id+"' rows='"+rows+"' cols='"+cols+"'>"+text+"</textarea><br />");
}

function insert_radio(id,name,text){
	my_editor.selection.setContent("<label for='"+id+"'>"+value+"</label>"+"<input type='radio' id='"+id+"' name='"+name+"' value='"+value+"'></input><br />");
}

function insert_checkbox(id,name,value){
	my_editor.selection.setContent("<input type='checkbox' id='"+id+"' name='"+name+"' value='"+value+"'>"+value+"</input><br />");
}

