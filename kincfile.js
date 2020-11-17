let projectName = 'Sample';
let p = new Project(projectName + '_' + Project.platform);

Options = {};
Options.enableDemo = true;

p.addFiles(
	'src/app.cpp',
	'src/pch.h',
	'src/assets/data.h',
	'src/tests/gfxcommandstest.cpp',
	'src/tests/gfxcommandstest.h',
);

p.addIncludeDir('src');
p.addProject("../../engine/");

p.addDefine('STBI_ONLY_PNG');

let modules = [
	//'hfsm2',
	'entt',
	'imgui',
	'box2d',
	//'soloud'
];

let deps = {
	'box2d' : function (){
		p.addFiles(
			'src/tests/box2dtest.h',
			'src/tests/box2dtest.cpp'
		);
	},
	'entt' : function (){
		p.addFiles(
			'src/tests/ecstest.h',
			'src/tests/ecstest.cpp'
		);
	},
	'imgui' : function (){
		p.addFiles(
			'src/tests/imguitest.cpp'
		);
	},
};

let mlen = modules.length;
for (mod = 0; mod < mlen; ++mod){
	let modname = modules[mod].toUpperCase();
	console.log('{//' +  modules[mod]);
	p.addProject('../../thirdparty/' + modules[mod] + '/', Options);
	let modDef = 'KISS_' + modname;
	p.addDefine(modDef);
	console.log('#define ' + modDef);
	console.log('}');
	d = deps[modules[mod]];
	if (d != null) d();
}

p.c11 = true;

p.setDebugDir('bin/'+Project.platform);
//console.log(GraphicsApi)
switch (platform) {
	case "html5":
		p.setDebugDir('html5');
		p.addDefine("NDEBUG");
		break;
	case "android":
		p.targetOptions.android.package = 'it.gamely.' + projectName;
		p.targetOptions.android.versionCode = 1;
		p.targetOptions.android.versionName = '1.0';
		p.targetOptions.android.screenOrientation = 'landscape';
		p.targetOptions.android.compileSdkVersion = 29;
		p.targetOptions.android.minSdkVersion = 14;
		p.targetOptions.android.targetSdkVersion = 29;
		break;
	default:
		//p.setDebugDir('Deployment');
		break;
}
resolve(p);