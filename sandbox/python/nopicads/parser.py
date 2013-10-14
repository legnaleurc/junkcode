import re
import sys

from markdown.blockprocessors import BlockProcessor
from markdown.extensions import Extension
from markdown import markdown


class ChangeLogProcessor(BlockProcessor):

    def __init__(self, parser, pack):
        BlockProcessor.__init__(self, parser)

        self.pack = pack
        self.first = True

    def test(self, parent, block):
        return self.first

    def run(self, parent, blocks):
        h = blocks[0]
        b = blocks[1]
        self.pack.setChangeLog(h, b)
        self.first = False


class ChangeLogExtension(Extension):

    def __init__(self, pack):
        super(ChangeLogExtension, self).__init__()

        self.pack = pack

    def extendMarkdown(self, md, md_globals):
        clp = ChangeLogProcessor(md.parser, self.pack)
        md.parser.blockprocessors.add('changelog', clp, '>empty')


class ReadMeProcessor(BlockProcessor):

    def __init__(self, parser, pack):
        BlockProcessor.__init__(self, parser)

        self.pack = pack
        self.first = True

    def test(self, parent, block):
        return self.first and block == '## Supported Sites'

    def run(self, parent, blocks):
        a = blocks[1]
        a = a.splitlines()
        c = []
        d = 0
        for b in a:
            if b == '* else':
                pass
            elif b[0] == '*':
                c.append(b)
            else:
                d = d + 1
        c = '\n'.join(c)
        self.pack.setReadMe(c, d)
        self.first = False


class ReadMeExtension(Extension):

    def __init__(self, pack):
        super(ReadMeExtension, self).__init__()

        self.pack = pack

    def extendMarkdown(self, md, md_globals):
        rmp = ReadMeProcessor(md.parser, self.pack)
        md.parser.blockprocessors.add('readme', rmp, '>empty')


class USOProcessor(BlockProcessor):

    def __init__(self, parser, pack):
        BlockProcessor.__init__(self, parser)

        self.pack = pack

    def test(self, parent, block):
        if block == '### Changelog':
            self.parser.state.set('cl')
        elif block == '### Supported sites':
            self.parser.state.set('rm')
        return True

    def run(self, parent, blocks):
        if self.parser.state.isstate('cl'):
            self.parser.state.reset()
            a = blocks.pop(0)
            self.pack.addBlock(a)
            blocks.pop(0)
            self.pack.addBlock(self.pack.cl_head)
            blocks.pop(0)
            self.pack.addBlock(self.pack.cl_body)
        elif self.parser.state.isstate('rm'):
            self.parser.state.reset()
            a = blocks.pop(0)
            self.pack.addBlock(a)
            blocks.pop(0)
            self.pack.addBlock(self.pack.rm_group)
            a = blocks.pop(0)
            print a
            a = re.sub(r'\d+( sites)', '{0}{1}'.format(self.pack.rm_count, r'\1'), a)
            print a
            self.pack.addBlock(a)
        else:
            a = blocks.pop(0)
            self.pack.addBlock(a)


class USOExtension(Extension):

    def __init__(self, pack):
        super(USOExtension, self).__init__()

        self.pack = pack

    def extendMarkdown(self, md, md_globals):
        rmp = USOProcessor(md.parser, self.pack)
        md.parser.blockprocessors.add('uso', rmp, '>empty')


class Pack(object):

    def __init__(self, cl, rm, uso):
        self.cl = cl
        self.rm = rm
        self.uso = uso
        self.cl_head = None
        self.cl_body = None
        self.rm_group = None
        self.rm_count = None
        self.blocks = []

    def setChangeLog(self, head, body):
        self.cl_head = head
        self.cl_body = body

    def setReadMe(self, group, count):
        self.rm_group = group
        self.rm_count = count

    def addBlock(self, block):
        self.blocks.append(block)

    def getResult(self):
        cle = ChangeLogExtension(self)
        unused = markdown(self.cl, [cle])
        rme = ReadMeExtension(self)
        unused = markdown(self.rm, [rme])
        usoe = USOExtension(self)
        unused = markdown(self.uso, [usoe])
        unused = '\n\n'.join(self.blocks) + '\n'
        return unused


def main(argv):
    fin = open('CHANGELOG.md', 'r')
    cl = fin.read()
    fin.close()
    fin = open('README.md', 'r')
    rm = fin.read()
    fin.close()
    fin = open('uso.md', 'r')
    uso = fin.read()
    uso = uso.decode('utf-8')
    fin.close()
    pack = Pack(cl, rm, uso)
    print pack.getResult()


if __name__ == '__main__':
    sys.exit(main(sys.argv))
