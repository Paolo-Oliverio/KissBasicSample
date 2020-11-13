let projectName = 'Sample';
let p = new Project(projectName + '_' + Project.platform);

p.addFile('src/**');
p.addIncludeDir('src');
p.addProject("../../engine/");

let modules = [
	//'hfsm2',
	'entt',
	'imgui',
	'box2d'
];
let mlen = modules.length;
for (i = 0; i < mlen; ++i){
	p.addProject('../../thirdparty/' + modules[i] + '/');
}	

p.c11 = true;

p.setDebugDir('bin/'+Project.platform);
//console.log(GraphicsApi)
switch (Project.platform) {
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