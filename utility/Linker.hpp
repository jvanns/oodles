#ifndef OODLES_LINKER_HPP
#define OODLES_LINKER_HPP

namespace oodles {

struct Link; // Forward declaration for Linker

class Linker
{
    public:
        /* Member functions/methods */
        Linker();
        virtual ~Linker();
        
        bool extend_link_to(Linker *l);
    protected:
        /* Member variables/attributes */
        Link *coupling;
    private:
        /* Member functions/methods */
        void unlink();
        void link(Link *l);

        /* Friend class declarations */
        friend struct Link; // Link wants link() only
};

struct Link
{
    /* Member functions/methods */
    Link(Linker *left, Linker *right);
    Link(const Link &l);
    
    Link& operator= (const Link &l);
    Linker* complement_of(const Linker &source) const;
    
    /* Member variables/attributes */
    Linker *left, *right;
};

} // oodles

#endif
