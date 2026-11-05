# What a pass of conversions left open, counted by cause: where the units a machine could translate are stuck, then every
# refusal by category. Usage: python scripts/open-causes.py <folder with <area>/conversion/ledger.json>
import collections, glob, json, re, sys
root=sys.argv[1]
tot=collections.Counter(); why=collections.Counter(); ex={}
for f in glob.glob(root+'/*/conversion/ledger.json'):
    d=json.load(open(f,encoding='utf8'))
    done=set(d['done'])
    outs={o['module']:o for o in d['outcomes']}
    for u in d['units']:
        tot[u['disposition']]+=1
        if u['disposition']!='translate': continue
        if u['id'] in done: tot['T done']+=1; continue
        o=outs.get(u['file'])
        if o is None: tot['T no module spec']+=1; why['no spec: kind '+u['kind']]+=1; continue
        if o['accepted']:
            tot['T module accepted, unit open']+=1
            why['acc: kind '+u['kind']]+=1
        else:
            tot['T module refused']+=1
            r=o['refusals'][0] if o['refusals'] else ''
            k=re.sub(r'.*\.ts:\d+: ','',r)[:60]; k=re.sub(r'^\w+: ','',k)[:50]
            why['ref: '+k]+=1; ex.setdefault(k,u['file'])
print(tot)
for k,n in why.most_common(40): print(n,k)

cat = collections.Counter(); mods = collections.Counter(); pairs = collections.Counter(); ex = {}
PATS = [
 ('shapes meet', r'values of different shapes meet \((.{0,60})'),
 ('union of different types', r'a union of different types'),
 ('name outside feature', r'a name outside the feature[^:]*: (\S+)'),
 ('no C++ form expr', r'no C\+\+ form for this expression: (.{0,30})'),
 ('member of', r'a member of (\S+)'),
 ('method of', r'a method (\w+) of'),
 ('Array.', r'Array\.(\w+)'),
 ('statement unknown', r'a statement the translator does not know: (\w+)'),
 ('throw', r'a throw with no pattern'),
 ('no field', r'has no field'),
 ('call unknown', r'a call the translator does not know'),
 ('js:: missing', r"no (?:member|type|template) named '(\w+)' in namespace 'js'"),
 ('does-not-compile', r'does-not-compile'),
 ('new X', r'new (\w+)'),
 ('type no form', r'no C\+\+ form'),
 ('differs from another', r'differs from another'),
 ('edit', r'an edit used as a value|reads an index'),
]
seen=0
for f in glob.glob(root + '/*/conversion/ledger.json'):
    d = json.load(open(f, encoding='utf8'))
    for o in d.get('outcomes', []):
        rs = o.get('refusals', [])
        if not o.get('accepted'): mods['refused'] += 1
        else: mods['accepted'] += 1
        for r in rs:
            seen+=1
            hit=False
            for name,p in PATS:
                ms = re.findall(p, r)
                if ms:
                    cat[name]+=len(ms); hit=True
                    ex.setdefault(name, r[:220])
                    if name=='shapes meet':
                        for m in ms: pairs[m.split(')')[0][:60]]+=1
                    if name in ('name outside feature','Array.','method of','member of','js:: missing','no C++ form expr','new X','statement unknown'):
                        for m in ms: pairs[name+' '+m[:30]]+=1
            if not hit: cat['other']+=1; ex.setdefault('other', r[:220])
print(mods, 'refusal strings', seen)
for k,n in cat.most_common(): print(f'{n:6d} {k}\n       {ex.get(k,"")[:200]}')
print('---')
for k,n in pairs.most_common(60): print(f'{n:6d} {k}')
