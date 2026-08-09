const $ = selector => document.querySelector(selector);
const canvas = $('#canvas');
const ctx = canvas.getContext('2d');

const bindingCatalog = [
  ['仪表信息','游戏名称','rb.dashboard.game','RaceBear Simulator','text'],
  ['仪表信息','车辆名称','rb.dashboard.vehicle','GT Prototype','text'],
  ['仪表信息','赛道名称','rb.dashboard.track','RaceBear Circuit','text'],
  ['比赛信息','比赛位置','rb.dashboard.position',3],
  ['比赛信息','遥测已连接','rb.dashboard.active',1],
  ['比赛信息','当前圈数','rb.vehicle.lap',8],
  ['比赛信息','总圈数','rb.vehicle.lap.max',20],
  ['比赛信息','当前圈时间','rb.vehicle.lap.time',72.438],
  ['比赛信息','最佳圈时间','rb.vehicle.best.lap.time',69.812],
  ['比赛信息','上一圈时间','rb.vehicle.last.lap.time',70.246],
  ['比赛信息','当前赛段','rb.vehicle.sector',2],
  ['比赛信息','赛道进度','rb.vehicle.progress',0.63],
  ['比赛信息','已行驶距离','rb.vehicle.track.distance',3125],
  ['比赛信息','赛道总长度','rb.vehicle.track.total.distance',4960],
  ['车辆','车速 km/h','rb.dashboard.speed_kmh',186],
  ['车辆','车速 m/s','rb.vehicle.speed',51.67],
  ['车辆','挡位','rb.vehicle.gear',4],
  ['车辆','最大挡位','rb.vehicle.gear.max',6],
  ['车辆','发动机转速','rb.vehicle.engine.rpm',7340],
  ['车辆','最大转速','rb.vehicle.rpm.max',9000],
  ['车辆','发动机扭矩','rb.vehicle.engine.torque',510],
  ['车辆','燃油量','rb.vehicle.fuel',42.5],
  ['车辆','燃油百分比','rb.vehicle.fuel.percent',0.54],
  ['车辆','燃油上限','rb.vehicle.fuel.max',78],
  ['车辆','涡轮压力','rb.vehicle.turbo',1.4],
  ['车辆','DRS 状态','rb.vehicle.drs',1],
  ['车辆','维修区限速','rb.vehicle.pit_limiter',0],
  ['驾驶输入','油门','rb.vehicle.throttle',0.82],
  ['驾驶输入','刹车','rb.vehicle.brake',0.18],
  ['驾驶输入','离合器','rb.vehicle.clutch',0],
  ['驾驶输入','方向盘输入','rb.vehicle.steering',-0.24],
  ['驾驶输入','方向盘角度','rb.vehicle.steering.angle',-38],
  ['驾驶输入','手刹','rb.vehicle.handbrake',0],
  ['驾驶输入','FFB','rb.vehicle.ffb',0.46],
  ['车辆动态','横向加速度','rb.motion.lateral.acceleration',0.72],
  ['车辆动态','纵向加速度','rb.motion.longitudinal.acceleration',-0.38],
  ['车辆动态','垂直加速度','rb.motion.vertical.acceleration',0.15],
  ['车辆动态','横向速度','rb.motion.lateral.speed',1.8],
  ['车辆动态','纵向速度','rb.motion.longitudinal.speed',51.67],
  ['车辆动态','垂直速度','rb.motion.vertical.speed',0.2],
  ['车辆动态','俯仰角度','rb.motion.pitch.position',-0.03],
  ['车辆动态','横滚角度','rb.motion.roll.position',0.08],
  ['车辆动态','偏航角度','rb.motion.yaw.position',0.14],
  ['温度与压力','冷却液温度','rb.vehicle.coolant.temperature',92],
  ['温度与压力','机油温度','rb.vehicle.oil.temperature',104],
  ['温度与压力','机油压力','rb.vehicle.oil.pressure',4.6],
  ['温度与压力','环境温度','rb.vehicle.air.temperature',24],
  ['温度与压力','赛道温度','rb.vehicle.track.temperature',31],
  ['轮胎','左前胎温','rb.vehicle.front_left.core.temperature',83],
  ['轮胎','右前胎温','rb.vehicle.front_right.core.temperature',84],
  ['轮胎','左后胎温','rb.vehicle.rear_left.core.temperature',81],
  ['轮胎','右后胎温','rb.vehicle.rear_right.core.temperature',82],
  ['轮胎','左前磨损','rb.vehicle.front_left.wear',0.94],
  ['轮胎','右前磨损','rb.vehicle.front_right.wear',0.93],
  ['轮胎','左后磨损','rb.vehicle.rear_left.wear',0.96],
  ['轮胎','右后磨损','rb.vehicle.rear_right.wear',0.95],
  ['轮胎','左前胎压','rb.vehicle.front_left.pressure',26.4],
  ['轮胎','右前胎压','rb.vehicle.front_right.pressure',26.6],
  ['轮胎','左后胎压','rb.vehicle.rear_left.pressure',25.8],
  ['轮胎','右后胎压','rb.vehicle.rear_right.pressure',25.9],
  ['轮胎','左前打滑','rb.vehicle.front_left.slip',0.08],
  ['轮胎','右前打滑','rb.vehicle.front_right.slip',0.06],
  ['轮胎','左后打滑','rb.vehicle.rear_left.slip',0.12],
  ['轮胎','右后打滑','rb.vehicle.rear_right.slip',0.10],
  ['驾驶辅助','ABS 启用','rb.driver_assist.abs.enabled',1],
  ['驾驶辅助','ABS 激活','rb.driver_assist.abs.active',0],
  ['驾驶辅助','ABS 等级','rb.driver_assist.abs.level',4],
  ['驾驶辅助','TC 启用','rb.driver_assist.tc.enabled',1],
  ['驾驶辅助','TC 激活','rb.driver_assist.tc.active',0],
  ['驾驶辅助','TC 等级','rb.driver_assist.tc.level',3],
  ['运动平台','平台 Sway','rb.dashboard.rig.sway',0.12],
  ['运动平台','平台 Surge','rb.dashboard.rig.surge',-0.18],
  ['运动平台','平台 Heave','rb.dashboard.rig.heave',0.08],
  ['运动平台','平台 Yaw','rb.dashboard.rig.yaw',0.04],
  ['运动平台','平台 Roll','rb.dashboard.rig.roll',-0.07],
  ['运动平台','平台 Pitch','rb.dashboard.rig.pitch',0.11],
  ['派生效果','换挡冲击','rb.derived.gear.shift',0],
  ['派生效果','ABS 脉冲','rb.derived.abs.pulse',0],
  ['派生效果','TC 脉冲','rb.derived.tc.pulse',0],
  ['派生效果','锁胎','rb.derived.wheel.lock',0],
  ['派生效果','轮胎空转','rb.derived.wheel.spin',0],
  ['派生效果','路面冲击','rb.derived.road.impact',0.16],
  ['派生效果','路面纹理','rb.derived.road.texture',0.22],
  ['卡车','导航剩余距离','rb.truck.nav.distance',128400],
  ['卡车','导航剩余时间','rb.truck.nav.time',5420],
  ['卡车','道路限速','rb.truck.nav.speed.limit',80],
  ['卡车','续航里程','rb.truck.fuel.range',620],
  ['卡车','平均油耗','rb.truck.fuel.average.consumption',29.4],
  ['卡车','制动气压','rb.truck.brake.air.pressure',9.6],
  ['卡车','制动温度','rb.truck.brake.temperature',148],
  ['卡车','电瓶电压','rb.truck.battery.voltage',24.8],
  ['卡车','缓速器等级','rb.truck.retarder.level',2],
  ['卡车','累计里程','rb.truck.odometer',183420],
  ['飞行','攻角 AOA','rb.vehicle.aoa',4.2],
  ['飞行','侧滑角 AOS','rb.vehicle.aos',0.8],
  ['飞行','海拔高度','rb.vehicle.elevation.msl',1520],
  ['飞行','离地高度','rb.vehicle.elevation.ground',430],
  ['飞行','襟翼位置','rb.vehicle.flaps',0.25],
  ['飞行','起落架位置','rb.vehicle.landing.gear',1],
  ['飞行','减速板位置','rb.vehicle.speed.brakes',0],
  ['飞行','空气压力','rb.vehicle.air.pressure',84.6],
  ['授权','允许硬件输出','rb.dashboard.license_output_allowed',1]
].map(([group,label,key,value,type='number']) => ({group,label,key,value,type}));

const bindingByKey = new Map(bindingCatalog.map(item => [item.key,item]));
const sim = Object.fromEntries(bindingCatalog.map(item => [item.key,item.value]));
const elementLabels = {text:'文本',rect:'矩形',ellipse:'椭圆',line:'线条',bar:'进度条',image:'图片',gauge:'圆形仪表'};
const toolTypes = ['text','rect','ellipse','line','bar','gauge','image'];
let doc;
let currentPage = 0;
let selected = new Set();
let primarySelection = -1;
let drag = null;
let marquee = null;

function fresh() {
  return {formatVersion:1,id:'customer.dashboard',name:'My Dashboard',author:'',description:'',designWidth:854,designHeight:480,backgroundColor:'#FF101318',assets:{},pages:[{name:'Main',elements:[]}]};
}
function page() { return doc.pages[currentPage]; }
function elements() { return page().elements; }
function selectedIndices() { return [...selected].filter(index=>index>=0&&index<elements().length).sort((a,b)=>a-b); }
function selectedElements() { return selectedIndices().map(index=>elements()[index]); }
function primaryElement() { return primarySelection>=0 ? elements()[primarySelection] : null; }
function isSelected(index) { return selected.has(index); }
function clearSelection() { selected.clear(); primarySelection=-1; }
function selectOnly(index) { selected=new Set(index>=0?[index]:[]); primarySelection=index; }
function selectMany(indices,primary=indices.at(-1)??-1) { selected=new Set(indices); primarySelection=primary; }
function toggleSelection(index) { if(selected.has(index)){selected.delete(index);if(primarySelection===index)primarySelection=selectedIndices().at(-1)??-1}else{selected.add(index);primarySelection=index} }
function color(value='#FFFFFFFF') { return value.length === 9 ? '#'+value.slice(3) : value; }
function argb(value) { return '#FF'+value.slice(1).toUpperCase(); }
function bindingValue(key) { return key ? (sim[key] ?? 0) : 0; }
function scaledValue(element) {
  const raw = bindingValue(element.binding);
  return typeof raw === 'number' ? raw * (element.scale ?? 1) + (element.offset ?? 0) : raw;
}
function visible(element) {
  if (!element.visibleBinding) return true;
  const value = Number(bindingValue(element.visibleBinding));
  return value >= (element.visibleMinimum ?? -999999) && value <= (element.visibleMaximum ?? 999999);
}
function zoneColor(element,value) {
  let result = element.color || '#FFFFFFFF';
  [...(element.zones || [])].sort((a,b)=>a.minimum-b.minimum).forEach(zone => { if (value >= zone.minimum) result = zone.color; });
  return result;
}
function formatted(element) {
  const raw = scaledValue(element);
  if (!element.binding) return element.text || 'Text';
  if (typeof raw === 'string') return raw;
  if (element.format === 'gear') return raw < 0 ? 'R' : raw === 0 ? 'N' : String(Math.round(raw));
  if (element.format === 'time') {
    const safe = Math.max(0,raw), minutes = Math.floor(safe/60), seconds = safe-minutes*60;
    return `${minutes}:${seconds.toFixed(element.decimals ?? 3).padStart(2+(element.decimals ? element.decimals+1 : 0),'0')}`;
  }
  const value = element.format === 'percent' ? raw*100 : raw;
  const body = element.format === 'integer' ? Math.round(value) : Number(value).toFixed(element.decimals ?? 0);
  return `${element.prefix || ''}${body}${element.format === 'percent' && !element.suffix ? '%' : element.suffix || ''}`;
}
function normalizeDocument(value) {
  value.assets ||= {};
  value.pages ||= [{name:'Main',elements:[]}];
  value.pages.forEach((item,index) => { item.name ||= `Page ${index+1}`; item.elements ||= []; });
  return value;
}
function nextPosition(width,height) {
  const padding=24;
  for(let y=padding;y+height<=doc.designHeight-padding;y+=32){
    for(let x=padding;x+width<=doc.designWidth-padding;x+=32){
      const occupied=elements().some(element=>x<element.x+element.width+12&&x+width+12>element.x&&y<element.y+element.height+12&&y+height+12>element.y);
      if(!occupied)return{x,y};
    }
  }
  const offset=(elements().length*18)%120;
  return{x:padding+offset,y:padding+offset};
}

function draw() {
  if(canvas.width!==doc.designWidth)canvas.width=doc.designWidth;
  if(canvas.height!==doc.designHeight)canvas.height=doc.designHeight;
  ctx.fillStyle = color(doc.backgroundColor);
  ctx.fillRect(0,0,canvas.width,canvas.height);
  elements().forEach((element,index) => {
    const conditionVisible=visible(element);
    const showHidden=$('#showHiddenElements')?.checked!==false;
    if (!conditionVisible && !showHidden && !isSelected(index)) return;
    ctx.save();
    if(!conditionVisible)ctx.globalAlpha=isSelected(index)?.55:.22;
    const number = Number(scaledValue(element)) || 0;
    const resolvedColor = color(zoneColor(element,number));
    ctx.fillStyle = resolvedColor;
    ctx.strokeStyle = color(element.borderColor || element.color || '#FFFFFFFF');
    ctx.lineWidth = element.lineWidth || 1;
    if (element.type === 'rect') {
      ctx.fillRect(element.x,element.y,element.width,element.height);
      if (element.borderColor) ctx.strokeRect(element.x,element.y,element.width,element.height);
    } else if (element.type === 'ellipse') {
      ctx.beginPath(); ctx.ellipse(element.x+element.width/2,element.y+element.height/2,element.width/2,element.height/2,0,0,Math.PI*2); ctx.fill();
      if (element.borderColor) ctx.stroke();
    } else if (element.type === 'line') {
      ctx.beginPath(); ctx.moveTo(element.x,element.y); ctx.lineTo(element.x+element.width,element.y+element.height); ctx.strokeStyle=resolvedColor; ctx.stroke();
    } else if (element.type === 'bar') {
      ctx.fillStyle = color(element.backgroundColor || '#FF252C35'); ctx.fillRect(element.x,element.y,element.width,element.height);
      const ratio = Math.max(0,Math.min(1,(number-(element.minimum ?? 0))/((element.maximum ?? 1)-(element.minimum ?? 0))));
      ctx.fillStyle = resolvedColor;
      if (element.vertical) ctx.fillRect(element.x,element.y+element.height*(1-ratio),element.width,element.height*ratio);
      else ctx.fillRect(element.x,element.y,element.width*ratio,element.height);
    } else if (element.type === 'text') {
      ctx.fillStyle = resolvedColor;
      ctx.font = `${element.bold ? '700' : '400'} ${element.fontSize || 24}px ${element.fontFamily || 'Segoe UI'}`;
      ctx.textAlign = element.align || 'center'; ctx.textBaseline = 'middle';
      const x = element.align === 'left' ? element.x : element.align === 'right' ? element.x+element.width : element.x+element.width/2;
      ctx.fillText(formatted(element),x,element.y+element.height/2,element.width);
    } else if (element.type === 'gauge') {
      const diameter=Math.min(element.width,element.height),cx=element.x+element.width/2,cy=element.y+element.height/2,radius=diameter*.46;
      const start=(element.startAngle??135)*Math.PI/180,end=(element.endAngle??405)*Math.PI/180;
      const ratio=Math.max(0,Math.min(1,(number-(element.minimum??0))/((element.maximum??1)-(element.minimum??0))));
      ctx.fillStyle=color(element.backgroundColor||'#FF171C23');ctx.beginPath();ctx.arc(cx,cy,diameter/2,0,Math.PI*2);ctx.fill();
      ctx.lineWidth=element.lineWidth||10;ctx.lineCap='round';ctx.strokeStyle=color(element.borderColor||'#FF37414E');ctx.beginPath();ctx.arc(cx,cy,radius,start,end);ctx.stroke();
      ctx.strokeStyle=resolvedColor;ctx.beginPath();ctx.arc(cx,cy,radius,start,start+(end-start)*ratio);ctx.stroke();
      ctx.lineWidth=Math.max(1,(element.lineWidth||10)*.35);ctx.strokeStyle=color(element.borderColor||'#FF94A3B8');
      for(let tick=0;tick<=(element.tickCount??9);tick++){const angle=start+(end-start)*tick/(element.tickCount??9);ctx.beginPath();ctx.moveTo(cx+Math.cos(angle)*radius*.82,cy+Math.sin(angle)*radius*.82);ctx.lineTo(cx+Math.cos(angle)*radius*.96,cy+Math.sin(angle)*radius*.96);ctx.stroke()}
      const needle=start+(end-start)*ratio;ctx.lineWidth=Math.max(2,(element.lineWidth||10)*.5);ctx.strokeStyle=color(element.needleColor||'#FFFFFFFF');ctx.beginPath();ctx.moveTo(cx,cy);ctx.lineTo(cx+Math.cos(needle)*radius*.72,cy+Math.sin(needle)*radius*.72);ctx.stroke();
      ctx.fillStyle=color(element.needleColor||'#FFFFFFFF');ctx.beginPath();ctx.arc(cx,cy,diameter*.035,0,Math.PI*2);ctx.fill();
      if(element.showValue!==false){ctx.font=`${element.bold?'700':'400'} ${element.fontSize||24}px ${element.fontFamily||'Bahnschrift'}`;ctx.textAlign='center';ctx.textBaseline='middle';ctx.fillText(formatted(element),cx,element.y+diameter*.71,diameter*.58)}
    } else if (element.type === 'image' && doc.assets[element.asset]) {
      const image = new Image(); image.onload = () => ctx.drawImage(image,element.x,element.y,element.width,element.height); image.src = doc.assets[element.asset];
    }
    if (isSelected(index)) {
      ctx.globalAlpha=1;ctx.strokeStyle=index===primarySelection?'#5ce0b7':'#ffc95e';ctx.lineWidth=index===primarySelection?2:1.5;ctx.setLineDash(index===primarySelection?[6,4]:[3,3]);ctx.strokeRect(element.x-2,element.y-2,element.width+4,element.height+4);
      if(!conditionVisible){ctx.fillStyle='#c99e4b';ctx.font='10px Segoe UI';ctx.textAlign='left';ctx.textBaseline='bottom';ctx.fillText('条件隐藏',element.x,element.y-5)}
    }
    ctx.restore();
  });
  if(selected.size>1){const chosen=selectedElements(),left=Math.min(...chosen.map(e=>e.x)),top=Math.min(...chosen.map(e=>e.y)),right=Math.max(...chosen.map(e=>e.x+e.width)),bottom=Math.max(...chosen.map(e=>e.y+e.height));ctx.save();ctx.strokeStyle='#ffc95e';ctx.lineWidth=1;ctx.setLineDash([8,5]);ctx.strokeRect(left-6,top-6,right-left+12,bottom-top+12);ctx.restore()}
  if(marquee){ctx.save();ctx.fillStyle='rgba(92,224,183,.10)';ctx.strokeStyle='#5ce0b7';ctx.lineWidth=1;ctx.setLineDash([5,4]);ctx.fillRect(marquee.x,marquee.y,marquee.width,marquee.height);ctx.strokeRect(marquee.x,marquee.y,marquee.width,marquee.height);ctx.restore()}
  $('#documentName').textContent = `${doc.name} · ${page().name}`;
  renderLayers();
  updateLayerButtons();
}

function baseElement(type) {
  const common = {type,name:elementLabels[type],x:80,y:70,width:220,height:70,color:'#FFFFFFFF',scale:1,offset:0,minimum:0,maximum:1};
  if (type === 'text') Object.assign(common,{text:'Text',fontSize:36,fontFamily:'Bahnschrift',align:'center',format:'number',decimals:0});
  if (type === 'rect' || type === 'ellipse') Object.assign(common,{color:'#FF2FD7A0',borderColor:'#00000000',lineWidth:1});
  if (type === 'line') Object.assign(common,{height:0,lineWidth:4});
  if (type === 'bar') Object.assign(common,{binding:'rb.vehicle.engine.rpm',minimum:0,maximum:9000,color:'#FF2FD7A0',backgroundColor:'#FF252C35',height:24});
  if (type === 'gauge') Object.assign(common,{name:'圆形仪表',width:260,height:260,binding:'rb.dashboard.speed_kmh',minimum:0,maximum:300,color:'#FF2FD7A0',backgroundColor:'#FF171C23',borderColor:'#FF37414E',needleColor:'#FFFFFFFF',lineWidth:11,startAngle:135,endAngle:405,tickCount:9,showValue:true,fontSize:26,fontFamily:'Bahnschrift',bold:true,format:'integer',decimals:0,suffix:' km/h'});
  return common;
}
function addElements(items) { const start=elements().length;elements().push(...items);selectMany(items.map((_,index)=>start+index));refresh(); }
function pushElement(element) { addElements([element]); }
function add(type) {
  const element = baseElement(type);
  Object.assign(element,nextPosition(element.width,element.height));
  if (type !== 'image') return pushElement(element);
  const input = document.createElement('input'); input.type='file'; input.accept='image/png,image/jpeg';
  input.onchange = () => { const file=input.files[0]; if(!file)return; const reader=new FileReader(); reader.onload=()=>{const key=`image${Object.keys(doc.assets).length+1}`;doc.assets[key]=reader.result;element.asset=key;element.name=file.name;pushElement(element)};reader.readAsDataURL(file); };
  input.click();
}
function addPreset(kind) {
  const presetSizes={rpm:[620,80],pedals:[300,64],tyres:[350,140],tyrewear:[350,140],status:[220,42],shiftlights:[324,30],warnings:[260,44],cluster:[740,280],raceinfo:[600,110],engineinfo:[560,88],motion:[620,150],digitalstrip:[720,86]};
  const [presetWidth,presetHeight]=presetSizes[kind]||((['speedometer','tachometer','fuel','coolant','turbo','steering'].includes(kind))?[260,260]:[340,110]);
  const {x,y}=nextPosition(presetWidth,presetHeight);
  if (kind === 'speed') return addElements([{...baseElement('text'),name:'车速',x,y,width:330,height:110,binding:'rb.dashboard.speed_kmh',format:'integer',suffix:' km/h',fontSize:58,bold:true,color:'#FF2FD7A0'}]);
  if (kind === 'gear') return addElements([{...baseElement('text'),name:'挡位',x,y,width:180,height:180,binding:'rb.vehicle.gear',format:'gear',fontSize:132,bold:true}]);
  if (kind === 'laptime') return addElements([{...baseElement('text'),name:'圈速',x,y,width:340,height:74,binding:'rb.vehicle.lap.time',format:'time',decimals:3,fontSize:42,bold:true}]);
  if (kind === 'rpm') {
    return addElements([{...baseElement('bar'),name:'转速条',x,y,width:620,height:28,binding:'rb.vehicle.engine.rpm',minimum:0,maximum:9000,zones:[{minimum:7000,color:'#FFFFC857'},{minimum:8200,color:'#FFFF5A5F'}]},{...baseElement('text'),name:'转速文字',x,y:y+34,width:620,height:40,binding:'rb.vehicle.engine.rpm',format:'integer',suffix:' RPM',fontSize:22,color:'#FF94A3B8'}]);
  }
  if (kind === 'pedals') {
    return addElements([{...baseElement('bar'),name:'油门',x,y,width:300,height:24,binding:'rb.vehicle.throttle',minimum:0,maximum:1,color:'#FF2FD7A0'},{...baseElement('bar'),name:'刹车',x,y:y+38,width:300,height:24,binding:'rb.vehicle.brake',minimum:0,maximum:1,color:'#FFFF5A5F'}]);
  }
  if (kind === 'tyres') {
    const keys=['front_left','front_right','rear_left','rear_right'];
    return addElements(keys.map((wheel,index)=>({...baseElement('text'),name:['左前胎温','右前胎温','左后胎温','右后胎温'][index],x:x+(index%2)*190,y:y+Math.floor(index/2)*76,width:160,height:58,binding:`rb.vehicle.${wheel}.core.temperature`,format:'integer',suffix:' °C',fontSize:25,bold:true,color:'#FFF2F5F7'})));
  }
  if (kind === 'tyrewear') {
    const keys=['front_left','front_right','rear_left','rear_right'];
    return addElements(keys.map((wheel,index)=>({...baseElement('text'),name:['左前磨损','右前磨损','左后磨损','右后磨损'][index],x:x+(index%2)*190,y:y+Math.floor(index/2)*76,width:160,height:58,binding:`rb.vehicle.${wheel}.wear`,format:'percent',fontSize:25,bold:true,color:'#FF55B7FF'})));
  }
  if (kind === 'status') {
    return addElements([{...baseElement('ellipse'),name:'连接指示灯',x,y,width:24,height:24,color:'#FF2FD7A0',visibleBinding:'rb.dashboard.active',visibleMinimum:1,visibleMaximum:1},{...baseElement('text'),name:'连接状态',x:x+34,y:y-8,width:180,height:40,text:'TELEMETRY',fontSize:17,align:'left',color:'#FF94A3B8'}]);
  }
  const gauges={speedometer:{name:'指针车速表',binding:'rb.dashboard.speed_kmh',maximum:300,suffix:' km/h',color:'#FF2FD7A0'},tachometer:{name:'指针转速表',binding:'rb.vehicle.engine.rpm',maximum:9000,suffix:' RPM',color:'#FFFFC857',zones:[{minimum:7000,color:'#FFFFC857'},{minimum:8200,color:'#FFFF5A5F'}]},fuel:{name:'油量表',binding:'rb.vehicle.fuel.percent',maximum:1,format:'percent',suffix:'',color:'#FF55B7FF'},coolant:{name:'水温表',binding:'rb.vehicle.coolant.temperature',minimum:40,maximum:130,suffix:' °C',color:'#FF55B7FF',zones:[{minimum:105,color:'#FFFFC857'},{minimum:118,color:'#FFFF5A5F'}]},turbo:{name:'涡轮表',binding:'rb.vehicle.turbo',minimum:0,maximum:3,suffix:' bar',decimals:1,color:'#FFA78BFA'},steering:{name:'方向盘角度表',binding:'rb.vehicle.steering.angle',minimum:-540,maximum:540,suffix:'°',color:'#FF55B7FF',startAngle:180,endAngle:360}};
  if(gauges[kind])return addElements([{...baseElement('gauge'),x,y,...gauges[kind]}]);
  if(kind==='shiftlights'){
    return addElements(Array.from({length:8},(_,index)=>({...baseElement('ellipse'),name:`换挡灯 ${index+1}`,x:x+index*42,y,width:28,height:28,color:index<5?'#FF2FD7A0':index<7?'#FFFFC857':'#FFFF5A5F',visibleBinding:'rb.vehicle.engine.rpm',visibleMinimum:5600+index*400,visibleMaximum:20000})));
  }
  if(kind==='warnings'){
    return addElements([['ABS','rb.driver_assist.abs.active','#FFFFC857'],['TC','rb.driver_assist.tc.active','#FFFFC857'],['PIT','rb.vehicle.pit_limiter','#FF55B7FF']].map(([label,binding,colorValue],index)=>({...baseElement('text'),name:`${label} 告警`,x:x+index*92,y,width:76,height:42,text:label,fontSize:18,bold:true,color:colorValue,visibleBinding:binding,visibleMinimum:1,visibleMaximum:1})));
  }
  if(kind==='cluster'){
    return addElements([{...baseElement('gauge'),name:'主车速表',x,y,width:260,height:260,binding:'rb.dashboard.speed_kmh',maximum:300,suffix:' km/h',color:'#FF2FD7A0'},{...baseElement('text'),name:'主挡位',x:x+280,y:y+50,width:160,height:160,binding:'rb.vehicle.gear',format:'gear',fontSize:118,bold:true},{...baseElement('gauge'),name:'主转速表',x:x+460,y,width:260,height:260,binding:'rb.vehicle.engine.rpm',maximum:9000,suffix:' RPM',color:'#FFFFC857',zones:[{minimum:7000,color:'#FFFFC857'},{minimum:8200,color:'#FFFF5A5F'}]}]);
  }
  if(kind==='raceinfo'){
    return addElements([{...baseElement('text'),name:'比赛位置',x,y,width:130,height:92,binding:'rb.dashboard.position',format:'integer',prefix:'P',fontSize:48,bold:true,color:'#FFFFC857'},{...baseElement('text'),name:'当前圈',x:x+145,y,width:130,height:92,binding:'rb.vehicle.lap',format:'integer',prefix:'LAP ',fontSize:28,bold:true},{...baseElement('text'),name:'当前圈时间',x:x+290,y,width:290,height:42,binding:'rb.vehicle.lap.time',format:'time',decimals:3,fontSize:27,bold:true},{...baseElement('text'),name:'最佳圈时间',x:x+290,y:y+48,width:290,height:38,binding:'rb.vehicle.best.lap.time',format:'time',decimals:3,prefix:'BEST ',fontSize:19,color:'#FF55B7FF'}]);
  }
  if(kind==='engineinfo'){
    return addElements([['冷却液','rb.vehicle.coolant.temperature',' °C'],['机油温度','rb.vehicle.oil.temperature',' °C'],['机油压力','rb.vehicle.oil.pressure',''],['燃油','rb.vehicle.fuel.percent','']].map(([label,binding,suffix],index)=>({...baseElement('text'),name:label,x:x+index*140,y,width:130,height:78,binding,format:binding.endsWith('percent')?'percent':'number',decimals:binding.includes('pressure')?1:0,suffix,fontSize:24,bold:true,color:index===3?'#FF55B7FF':'#FFF2F5F7'})));
  }
  if(kind==='motion'){
    const axes=[['Sway','rb.dashboard.rig.sway'],['Surge','rb.dashboard.rig.surge'],['Heave','rb.dashboard.rig.heave'],['Yaw','rb.dashboard.rig.yaw'],['Roll','rb.dashboard.rig.roll'],['Pitch','rb.dashboard.rig.pitch']];
    return addElements(axes.flatMap(([label,binding],index)=>[{...baseElement('text'),name:`${label} 标签`,x,y:y+index*24,width:70,height:20,text:label,fontSize:12,align:'left',color:'#FF94A3B8'},{...baseElement('bar'),name:label,x:x+76,y:y+index*24,width:520,height:16,binding,minimum:-1,maximum:1,color:'#FF55B7FF',backgroundColor:'#FF252C35'}]));
  }
  if(kind==='digitalstrip'){
    return addElements([{...baseElement('text'),name:'数字车速',x,y,width:260,height:78,binding:'rb.dashboard.speed_kmh',format:'integer',suffix:' km/h',fontSize:45,bold:true,color:'#FF2FD7A0'},{...baseElement('text'),name:'数字挡位',x:x+270,y,width:100,height:78,binding:'rb.vehicle.gear',format:'gear',fontSize:60,bold:true},{...baseElement('text'),name:'数字转速',x:x+380,y,width:210,height:78,binding:'rb.vehicle.engine.rpm',format:'integer',suffix:' RPM',fontSize:25,bold:true,color:'#FFFFC857'},{...baseElement('text'),name:'油量',x:x+600,y,width:110,height:78,binding:'rb.vehicle.fuel.percent',format:'percent',fontSize:24,bold:true,color:'#FF55B7FF'}]);
  }
}

function renderPages() {
  const container=$('#pages'); container.innerHTML='';
  doc.pages.forEach((item,index)=>{const button=document.createElement('button');button.className='page'+(index===currentPage?' active':'');button.textContent=item.name;button.onclick=()=>{currentPage=index;clearSelection();refresh()};container.append(button)});
  $('#deletePage').disabled=doc.pages.length===1;
}
function renderLayers() {
  const list=$('#layers'); list.innerHTML='';
  [...elements()].map((value,index)=>[value,index]).reverse().forEach(([element,index])=>{
    const row=document.createElement('div');row.className='layer'+(isSelected(index)?' active':'')+(index===primarySelection?' primary-selection':'');
    row.innerHTML=`<span class="type">${element.type}</span><span class="name">${element.name || bindingByKey.get(element.binding)?.label || element.text || elementLabels[element.type]}</span><span class="visibility-state">${visible(element)?'':'隐藏'}</span>`;
    row.onclick=event=>{if(event.ctrlKey||event.metaKey||event.shiftKey)toggleSelection(index);else selectOnly(index);refresh()};list.append(row);
  });
  if (!elements().length) list.innerHTML='<div class="empty">当前页面没有图层。<br>切换到组件库添加元素。</div>';
  $('#layerCountBadge').textContent=elements().length;
}
function updateLayerButtons(){const indices=selectedIndices(),single=indices.length===1;$('#duplicateLayer').disabled=!indices.length;$('#layerUp').disabled=!single||indices[0]===elements().length-1;$('#layerDown').disabled=!single||indices[0]===0;$('#selectionSummary').textContent=indices.length?`已选择 ${indices.length} 个`:'未选择';$('#selectionCount').textContent=indices.length?`${indices.length} 个元素`:'未选择';const hiddenCount=selectedElements().filter(element=>!visible(element)).length;$('#hiddenSelectionHint').textContent=hiddenCount?`${hiddenCount} 个处于条件隐藏态`:'';document.querySelectorAll('[data-arrange]').forEach(button=>{const mode=button.dataset.arrange;button.disabled=(mode.startsWith('distribute')?indices.length<3:!indices.length)})}

function group(title) { const section=document.createElement('section'); section.className='property-group'; section.innerHTML=`<h2>${title}</h2>`; return section; }
function field(label,key,type='text',options) {
  const element=primaryElement(), row=document.createElement('label'); row.className='field'; row.innerHTML=`<span>${label}</span>`;
  let input;
  if (options) { input=document.createElement('select'); options.forEach(option=>{const value=Array.isArray(option)?option[0]:option,text=Array.isArray(option)?option[1]:option;input.add(new Option(text,value))}); }
  else { input=document.createElement('input'); input.type=type; }
  input.value=type==='color'?color(element[key] || '#FFFFFFFF'):element[key] ?? '';
  if(type==='checkbox') input.checked=!!element[key];
  input.oninput=()=>{element[key]=type==='number'?Number(input.value):type==='color'?argb(input.value):type==='checkbox'?input.checked:input.value;draw()};
  row.append(input); return row;
}
function applyBindingDefaults(element,key) {
  if (element.type !== 'text' && element.type !== 'gauge') return;
  element.prefix=''; element.suffix='';
  if (key === 'rb.vehicle.gear') { element.format='gear'; element.decimals=0; return; }
  if (key.includes('lap.time')) { element.format='time'; element.decimals=3; return; }
  if (key === 'rb.dashboard.speed_kmh') { element.format='integer'; element.decimals=0; element.suffix=' km/h'; return; }
  if (key.includes('rpm')) { element.format='integer'; element.decimals=0; element.suffix=' RPM'; return; }
  if (key.includes('temperature')) { element.format='integer'; element.decimals=0; element.suffix=' °C'; return; }
  if (key.includes('.wear') || key === 'rb.vehicle.throttle' || key === 'rb.vehicle.brake' || key === 'rb.vehicle.clutch' || key === 'rb.vehicle.fuel.percent' || key === 'rb.vehicle.progress') { element.format='percent'; element.decimals=0; return; }
  element.format='number'; element.decimals=1;
}
function bindingField(label,key) {
  const element=primaryElement(), wrapper=document.createElement('label'); wrapper.className='binding-field'; wrapper.innerHTML=`<span>${label}</span>`;
  const controls=document.createElement('div'); controls.className='binding-controls';
  const search=document.createElement('input'); search.placeholder='搜索中文名称或数据 key';
  const select=document.createElement('select');
  const hint=document.createElement('div'); hint.className='binding-key';
  const populate=term=>{
    const current=element[key] || '', normalized=term.trim().toLowerCase(); select.innerHTML=''; select.add(new Option('不绑定',''));
    if(current && !bindingByKey.has(current)) select.add(new Option(`当前文件：${current}`,current));
    const groups=new Map();
    const allowText=key==='binding'&&element.type==='text';
    bindingCatalog.filter(item=>(allowText||item.type==='number')&&(!normalized||`${item.group} ${item.label} ${item.key}`.toLowerCase().includes(normalized))).forEach(item=>{if(!groups.has(item.group)){const optgroup=document.createElement('optgroup');optgroup.label=item.group;groups.set(item.group,optgroup);select.append(optgroup)}groups.get(item.group).append(new Option(item.label,item.key))});
    select.value=current;
  };
  populate(''); hint.textContent=element[key] || '未绑定';
  search.oninput=()=>populate(search.value);
  select.onchange=()=>{element[key]=select.value;if(key==='binding'&&select.value){const info=bindingByKey.get(select.value);element.name=info?.label||element.name;applyBindingDefaults(element,select.value);properties()}hint.textContent=select.value||'未绑定';draw();renderLayers()};
  controls.append(search,select,hint); wrapper.append(controls); return wrapper;
}
function previewValueField(key) {
  const info=bindingByKey.get(key),row=document.createElement('label');row.className='field';row.innerHTML='<span>预览值</span>';
  const input=document.createElement('input');input.type=info?.type==='text'?'text':'number';if(input.type==='number')input.step='any';input.value=sim[key]??0;
  input.oninput=()=>{sim[key]=input.type==='number'?Number(input.value):input.value;draw()};row.append(input);return row;
}
function zoneEditor(element) {
  const section=group('颜色区间');
  const zones=element.zones ||= [];
  for(let index=0;index<2;index++){
    const row=document.createElement('div');row.className='swatch-row';
    const threshold=document.createElement('input');threshold.type='number';threshold.placeholder=`区间 ${index+1} 起点`;threshold.value=zones[index]?.minimum ?? '';
    const swatch=document.createElement('input');swatch.type='color';swatch.value=color(zones[index]?.color || (index?'#FFFF5A5F':'#FFFFC857'));
    threshold.oninput=()=>{if(threshold.value===''){zones.splice(index,1);properties();draw();return}zones[index]={minimum:Number(threshold.value),color:argb(swatch.value)};draw()};
    swatch.oninput=()=>{if(!zones[index])zones[index]={minimum:0,color:argb(swatch.value)};else zones[index].color=argb(swatch.value);draw()};
    row.append(threshold,swatch);section.append(row);
  }
  return section;
}
function properties() {
  const panel=$('#elementProperties'); panel.innerHTML='';
  const indices=selectedIndices();
  if(!indices.length){panel.innerHTML='<p class="hint">单击选择元素；按住 Ctrl 或 Shift 可以追加选择；在画布空白处拖动可以框选。方向键移动 1 像素，Shift + 方向键移动 10 像素。</p>';return}
  if(indices.length>1){panel.innerHTML=`<div class="multi-selection-panel"><div class="selection-title"><strong>已选择 ${indices.length} 个元素</strong><span>可以整体拖动、复制、删除，并使用顶部工具栏对齐或分布。</span></div><div class="batch-grid"><button data-batch="left">左对齐</button><button data-batch="right">右对齐</button><button data-batch="top">顶对齐</button><button data-batch="bottom">底对齐</button><button data-batch="distributeH">横向分布</button><button data-batch="distributeV">纵向分布</button><button data-batch="sameWidth">统一宽度</button><button data-batch="sameHeight">统一高度</button></div><div class="actionrow"><button id="duplicateProperty">复制所选</button><button id="deleteProperty" class="danger">删除所选</button></div></div>`;panel.querySelectorAll('[data-batch]').forEach(button=>button.onclick=()=>arrangeSelection(button.dataset.batch));$('#duplicateProperty').onclick=duplicateSelected;$('#deleteProperty').onclick=deleteSelected;return}
  const element=primaryElement();
  const identity=group('元素'); identity.append(field('名称','name'),field('类型','type','text',toolTypes.map(type=>[type,elementLabels[type]]))); panel.append(identity);
  const geometry=group('位置与尺寸'); [['X','x'],['Y','y'],['宽度','width'],['高度','height']].forEach(([label,key])=>geometry.append(field(label,key,'number'))); panel.append(geometry);
  const data=group('数据绑定'); data.append(bindingField('显示数据','binding'));if(element.binding)data.append(previewValueField(element.binding));data.append(bindingField('可见条件','visibleBinding'));
  if(element.visibleBinding){data.append(field('可见最小值','visibleMinimum','number'),field('可见最大值','visibleMaximum','number'))} panel.append(data);
  if(element.type==='text'){
    const content=group('文本');content.append(field('固定文本','text'),field('字体','fontFamily','text',[['Bahnschrift','Bahnschrift'],['Segoe UI','Segoe UI'],['Arial','Arial'],['Consolas','Consolas'],['Microsoft YaHei','微软雅黑']]),field('字号','fontSize','number'),field('粗体','bold','checkbox'),field('对齐','align','text',[['left','左'],['center','中'],['right','右']]));panel.append(content);
    const format=group('数值格式');format.append(field('格式','format','text',[['number','小数'],['integer','整数'],['percent','百分比'],['gear','挡位'],['time','时间']]),field('小数位','decimals','number'),field('前缀','prefix'),field('后缀','suffix'),field('倍率','scale','number'),field('偏移','offset','number'));panel.append(format);
  }
  if(element.type==='gauge'){
    const format=group('数值格式');format.append(field('格式','format','text',[['number','小数'],['integer','整数'],['percent','百分比'],['gear','挡位'],['time','时间']]),field('小数位','decimals','number'),field('前缀','prefix'),field('后缀','suffix'),field('倍率','scale','number'),field('偏移','offset','number'));panel.append(format);
  }
  const style=group('样式');style.append(field('主颜色','color','color'));
  if(element.type==='rect'||element.type==='ellipse'){style.append(field('边框颜色','borderColor','color'),field('边框宽度','lineWidth','number'))}
  if(element.type==='line') style.append(field('线宽','lineWidth','number'));
  if(element.type==='bar'){style.append(field('背景颜色','backgroundColor','color'),field('最小值','minimum','number'),field('最大值','maximum','number'),field('垂直方向','vertical','checkbox'),field('倍率','scale','number'),field('偏移','offset','number'))}
  if(element.type==='gauge'){style.append(field('表盘背景','backgroundColor','color'),field('刻度颜色','borderColor','color'),field('指针颜色','needleColor','color'),field('圆弧宽度','lineWidth','number'),field('最小值','minimum','number'),field('最大值','maximum','number'),field('起始角度','startAngle','number'),field('结束角度','endAngle','number'),field('刻度数量','tickCount','number'),field('显示数值','showValue','checkbox'),field('数值字号','fontSize','number'))}
  panel.append(style);
  if(element.type==='text'||element.type==='bar'||element.type==='gauge') panel.append(zoneEditor(element));
  if(element.type==='image'){
    const asset=group('图片');asset.append(field('资源','asset','text',Object.keys(doc.assets).map(key=>[key,key])));panel.append(asset);
  }
  const actions=document.createElement('div');actions.className='actionrow';actions.innerHTML='<button id="duplicateProperty">复制</button><button id="deleteProperty" class="danger">删除</button>';
  panel.append(actions);$('#duplicateProperty').onclick=duplicateSelected;$('#deleteProperty').onclick=deleteSelected;
}
function documentField(label,key,type='text') {
  const row=document.createElement('label');row.className=type==='textarea'?'field wide':'field';row.innerHTML=`<span>${label}</span>`;const input=document.createElement(type==='textarea'?'textarea':'input');if(type!=='textarea')input.type=type;input.value=type==='color'?color(doc[key]):doc[key]??'';input.oninput=()=>{doc[key]=type==='number'?Number(input.value):type==='color'?argb(input.value):input.value;draw()};row.append(input);return row;
}
function documentProperties() {
  const panel=$('#documentProperties');panel.innerHTML='';const info=group('仪表文件');info.append(documentField('ID','id'),documentField('名称','name'),documentField('作者','author'),documentField('说明','description','textarea'));panel.append(info);
  const canvasGroup=group('设计画布');
  const preset=document.createElement('label');preset.className='field';preset.innerHTML='<span>尺寸预设</span><select><option value="">自定义</option><option value="854x480">854 × 480 横屏</option><option value="480x854">480 × 854 竖屏</option><option value="800x480">800 × 480</option><option value="1280x720">1280 × 720</option><option value="1920x1080">1920 × 1080</option></select>';preset.lastChild.onchange=()=>{if(!preset.lastChild.value)return;[doc.designWidth,doc.designHeight]=preset.lastChild.value.split('x').map(Number);refresh()};
  canvasGroup.append(preset,documentField('宽度','designWidth','number'),documentField('高度','designHeight','number'),documentField('背景','backgroundColor','color'));panel.append(canvasGroup);
  const pageGroup=group('当前页面');const name=document.createElement('label');name.className='field';name.innerHTML='<span>页面名称</span><input>';name.lastChild.value=page().name;name.lastChild.oninput=()=>{page().name=name.lastChild.value||`Page ${currentPage+1}`;renderPages();draw()};pageGroup.append(name);panel.append(pageGroup);
}
function refresh(){renderPages();documentProperties();properties();draw()}

function duplicateSelected(){const indices=selectedIndices();if(!indices.length)return;const copies=indices.map(index=>{const copy=structuredClone(elements()[index]);copy.x+=12;copy.y+=12;copy.name=`${copy.name||elementLabels[copy.type]} 副本`;return copy});addElements(copies)}
function deleteSelected(){const indices=selectedIndices();if(!indices.length)return;[...indices].reverse().forEach(index=>elements().splice(index,1));clearSelection();refresh()}
function moveLayer(delta){const indices=selectedIndices();if(indices.length!==1)return;const index=indices[0],target=index+delta;if(target<0||target>=elements().length)return;[elements()[index],elements()[target]]=[elements()[target],elements()[index]];selectOnly(target);refresh()}
function arrangeSelection(mode){const chosen=selectedElements();if(!chosen.length)return;const left=Math.min(...chosen.map(e=>e.x)),top=Math.min(...chosen.map(e=>e.y)),right=Math.max(...chosen.map(e=>e.x+e.width)),bottom=Math.max(...chosen.map(e=>e.y+e.height)),centerX=(left+right)/2,centerY=(top+bottom)/2;if(mode==='left')chosen.forEach(e=>e.x=chosen.length===1?0:left);if(mode==='right')chosen.forEach(e=>e.x=(chosen.length===1?doc.designWidth:right)-e.width);if(mode==='hcenter')chosen.forEach(e=>e.x=Math.round((chosen.length===1?doc.designWidth/2:centerX)-e.width/2));if(mode==='top')chosen.forEach(e=>e.y=chosen.length===1?0:top);if(mode==='bottom')chosen.forEach(e=>e.y=(chosen.length===1?doc.designHeight:bottom)-e.height);if(mode==='vcenter')chosen.forEach(e=>e.y=Math.round((chosen.length===1?doc.designHeight/2:centerY)-e.height/2));if(mode==='sameWidth'){const width=primaryElement()?.width??chosen[0].width;chosen.forEach(e=>e.width=width)}if(mode==='sameHeight'){const height=primaryElement()?.height??chosen[0].height;chosen.forEach(e=>e.height=height)}if(mode==='distributeH'&&chosen.length>=3){const sorted=[...chosen].sort((a,b)=>a.x-b.x),outerLeft=sorted[0].x,outerRight=sorted.at(-1).x+sorted.at(-1).width,total=sorted.reduce((sum,e)=>sum+e.width,0),gap=(outerRight-outerLeft-total)/(sorted.length-1);let cursor=outerLeft;sorted.forEach(e=>{e.x=Math.round(cursor);cursor+=e.width+gap})}if(mode==='distributeV'&&chosen.length>=3){const sorted=[...chosen].sort((a,b)=>a.y-b.y),outerTop=sorted[0].y,outerBottom=sorted.at(-1).y+sorted.at(-1).height,total=sorted.reduce((sum,e)=>sum+e.height,0),gap=(outerBottom-outerTop-total)/(sorted.length-1);let cursor=outerTop;sorted.forEach(e=>{e.y=Math.round(cursor);cursor+=e.height+gap})}refresh()}

const basicComponents={text:['T','文本','固定文字或数据读数'],rect:['▰','矩形','背景、分隔和面板'],ellipse:['●','椭圆','灯光和圆形装饰'],line:['╱','线条','刻度与分隔线'],bar:['▰','进度条','水平或垂直数值条'],gauge:['◴','圆形仪表','动态圆弧和指针'],image:['IMG','图片','导入 PNG 或 JPEG']};
const presetComponents=[
  ['speed','readout','SPD','车速数字','大号数字车速'],['gear','readout','GEAR','挡位显示','R/N/1-8 挡位'],['rpm','readout','RPM','转速条','带红区颜色'],['laptime','readout','LAP','圈速计时','分钟与毫秒'],['pedals','status','P/B','油门与刹车','双踏板进度条'],['tyres','status','TEMP','四轮胎温','四轮温度读数'],['tyrewear','status','WEAR','四轮磨损','四轮磨损百分比'],['status','status','LINK','遥测状态','连接指示灯'],
  ['speedometer','gauge','KMH','指针车速表','动态圆弧车速'],['tachometer','gauge','RPM','指针转速表','带黄红转速区'],['fuel','gauge','FUEL','油量表','燃油百分比'],['coolant','gauge','TEMP','水温表','带高温区间'],['turbo','gauge','BAR','涡轮表','涡轮压力'],['steering','gauge','DEG','方向角度表','方向盘角度'],
  ['shiftlights','status','LED','换挡灯组','按转速逐级显示'],['warnings','status','WARN','驾驶告警灯','ABS、TC 与 PIT'],
  ['cluster','bundle','CLUSTER','双圆主仪表','车速、挡位和转速'],['raceinfo','bundle','RACE','比赛信息面板','位置、圈数与圈时'],['engineinfo','bundle','ENGINE','发动机状态栏','水温、油温、油压和燃油'],['motion','bundle','6DOF','运动平台监视','六轴实时数值条'],['digitalstrip','bundle','DIGI','数字主信息条','车速、挡位、转速和油量']
];
function componentButton(icon,label,description,category,handler,bundle=false){const button=document.createElement('button');button.className='component-button'+(bundle?' bundle':'');button.dataset.category=category;button.dataset.search=`${label} ${description}`.toLowerCase();button.innerHTML=`<span class="component-icon">${icon}</span><span class="component-text"><strong>${label}</strong><small>${description}</small></span>`;button.onclick=handler;return button}
toolTypes.forEach(type=>{const [icon,label,description]=basicComponents[type];$('#toolButtons').append(componentButton(icon,label,description,'basic',()=>add(type)))});
presetComponents.forEach(([kind,category,icon,label,description])=>$('#presetButtons').append(componentButton(icon,label,description,category,()=>addPreset(kind),category==='bundle')));
function filterComponents(){const term=$('#componentSearch').value.trim().toLowerCase(),category=$('#componentCategory').value;document.querySelectorAll('.component-button').forEach(button=>button.hidden=(category!=='all'&&button.dataset.category!==category)||(term&&!button.dataset.search.includes(term)))}
$('#componentSearch').oninput=filterComponents;$('#componentCategory').onchange=filterComponents;
document.querySelectorAll('[data-left-panel]').forEach(button=>button.onclick=()=>{document.querySelectorAll('[data-left-panel]').forEach(item=>item.classList.toggle('active',item===button));$('#libraryPanel').hidden=button.dataset.leftPanel!=='library';$('#layersPanel').hidden=button.dataset.leftPanel!=='layers'});

$('#duplicateLayer').onclick=duplicateSelected;$('#layerUp').onclick=()=>moveLayer(1);$('#layerDown').onclick=()=>moveLayer(-1);$('#selectAllLayers').onclick=()=>{selectMany(elements().map((_,index)=>index));refresh()};
$('#addPage').onclick=()=>{doc.pages.push({name:`Page ${doc.pages.length+1}`,elements:[]});currentPage=doc.pages.length-1;clearSelection();refresh()};
$('#deletePage').onclick=()=>{if(doc.pages.length===1)return;doc.pages.splice(currentPage,1);currentPage=Math.max(0,currentPage-1);clearSelection();refresh()};
document.querySelectorAll('[data-arrange]').forEach(button=>button.onclick=()=>arrangeSelection(button.dataset.arrange));
document.querySelectorAll('[data-panel]').forEach(button=>button.onclick=()=>{document.querySelectorAll('[data-panel]').forEach(item=>item.classList.toggle('active',item===button));$('#elementPanel').hidden=button.dataset.panel!=='element';$('#documentPanel').hidden=button.dataset.panel!=='document'});

function canvasPoint(event){const rect=canvas.getBoundingClientRect();return{x:(event.clientX-rect.left)*canvas.width/rect.width,y:(event.clientY-rect.top)*canvas.height/rect.height}}
canvas.onpointerdown=event=>{const point=canvasPoint(event),modifier=event.ctrlKey||event.metaKey||event.shiftKey,showHidden=$('#showHiddenElements').checked,hit=[...elements()].map((value,index)=>[value,index]).reverse().find(([element,index])=>(visible(element)||showHidden||isSelected(index))&&point.x>=element.x&&point.x<=element.x+element.width&&point.y>=element.y&&point.y<=element.y+element.height)?.[1]??-1;if(hit>=0){if(modifier)toggleSelection(hit);else if(!isSelected(hit))selectOnly(hit);if(isSelected(hit)){const origins=new Map(selectedIndices().map(index=>[index,{x:elements()[index].x,y:elements()[index].y}]));drag={mode:'move',start:point,origins};canvas.setPointerCapture(event.pointerId)}}else{const base=modifier?new Set(selected):new Set();if(!modifier)clearSelection();drag={mode:'marquee',start:point,base};marquee={x:point.x,y:point.y,width:0,height:0};canvas.setPointerCapture(event.pointerId)}refresh()};
canvas.onpointermove=event=>{if(!drag)return;const point=canvasPoint(event);if(drag.mode==='move'){const dx=point.x-drag.start.x,dy=point.y-drag.start.y;drag.origins.forEach((origin,index)=>{elements()[index].x=Math.round(origin.x+dx);elements()[index].y=Math.round(origin.y+dy)})}else{const left=Math.min(drag.start.x,point.x),top=Math.min(drag.start.y,point.y),right=Math.max(drag.start.x,point.x),bottom=Math.max(drag.start.y,point.y),showHidden=$('#showHiddenElements').checked;marquee={x:left,y:top,width:right-left,height:bottom-top};const hits=elements().map((element,index)=>({element,index})).filter(({element})=>(visible(element)||showHidden)&&element.x<right&&element.x+element.width>left&&element.y<bottom&&element.y+element.height>top).map(({index})=>index);selectMany([...new Set([...drag.base,...hits])],hits.at(-1)??primarySelection)}draw()};
canvas.onpointerup=()=>{drag=null;marquee=null;refresh()};
document.addEventListener('keydown',event=>{if(['INPUT','SELECT','TEXTAREA'].includes(document.activeElement.tagName))return;if(event.key==='a'&&(event.ctrlKey||event.metaKey)){event.preventDefault();selectMany(elements().map((_,index)=>index));refresh();return}if(!selected.size)return;const step=event.shiftKey?10:1;if(event.key==='Delete'){deleteSelected();return}if(event.key.startsWith('Arrow')){selectedElements().forEach(element=>{if(event.key==='ArrowLeft')element.x-=step;if(event.key==='ArrowRight')element.x+=step;if(event.key==='ArrowUp')element.y-=step;if(event.key==='ArrowDown')element.y+=step});event.preventDefault();refresh()}});

$('#zoom').oninput=event=>{canvas.style.width=`${event.target.value}%`;$('#zoomValue').textContent=`${event.target.value}%`};
document.querySelectorAll('[data-sim]').forEach(input=>input.oninput=()=>{sim[input.dataset.sim]=Number(input.value);draw()});$('#showHiddenElements').onchange=draw;
$('#newFile').onclick=()=>{doc=fresh();currentPage=0;clearSelection();refresh()};
$('#openFile').onchange=event=>{const file=event.target.files[0];if(!file)return;const reader=new FileReader();reader.onload=()=>{try{const value=JSON.parse(reader.result);if(value.formatVersion!==1||!value.pages?.length)throw new Error();doc=normalizeDocument(value);currentPage=0;clearSelection();refresh()}catch{alert('不是有效的 .rbdash V1 文件')}};reader.readAsText(file)};
$('#exportFile').onclick=()=>{if(!/^[A-Za-z0-9._-]+$/.test(doc.id)){alert('ID 只能包含字母、数字、点、下划线和短横线');return}if(doc.designWidth<64||doc.designWidth>4096||doc.designHeight<64||doc.designHeight>4096){alert('画布尺寸必须在 64 到 4096 之间');return}const blob=new Blob([JSON.stringify(doc,null,2)],{type:'application/json'}),anchor=document.createElement('a');anchor.href=URL.createObjectURL(blob);anchor.download=`${doc.id}.rbdash`;anchor.click();URL.revokeObjectURL(anchor.href)};

doc=fresh();
addPreset('speed');
