#include "include/qlSim.h"
namespace
{
class alpha60 : public AlphaBase
{
    public:
    explicit alpha60(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),   
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")),
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")),
        Num1(cfg->getAttributeIntDefault("para1",10))
    {
    }
    void generate(int di) override
    {
	   vector<float> col1(GLOBAL::Instruments.size(),nan);
       vector<float> col2(GLOBAL::Instruments.size(),nan);

	   for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c2(Num1,nan);
                if(!(GLOBALFUNC::equal(high[di-delay][ii],low[di-delay][ii])))
                {
                col1[ii]=(((close[di-delay][ii]-low[di-delay][ii])-(high[di-delay][ii]-close[di-delay][ii]))/(high[di-delay][ii]-low[di-delay][ii]))*vol[di-delay][ii];
                }
                for (int i = 0; i< Num1; ++i)
                    {
                        c2[i]=close[di-delay-i][ii];
                     }
        
                col2[ii]=float(ts_argmax(c2));
           }
       }

       QL_Oputils::rank(col1);
       QL_Oputils::rank(col2);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
		      alpha[ii]=-(1.0*(2.0*(col1[ii]/QL_Oputils::sum(col1))-(col2[ii]/QL_Oputils::sum(col2))));
            }
        }
        return;
    }
    

      int ts_argmax(vector<float> &x)
    {
        
        int index_s = 0;
        float max_s = x[0];
        int num = 0;
        for(vector<float>::iterator it  = x.begin(); it != x.end(); ++it)  
        {  

            if (QL_Oputils::QL_less (max_s, *it))
            {
                max_s = *it;
                index_s = num;          
            }
            num = num + 1;
        }
        
        return index_s;
    }
    void checkPointSave(boost::archive::binary_oarchive &ar) 
    {
        ar & *this;
    }
    void checkPointLoad(boost::archive::binary_iarchive &ar)
    {
        ar & *this;
    }
    std::string version() const
    {
        return GLOBALCONST::VERSION;
    }
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int/*file_version*/)
        {
        }
        const QL_MATRIX<QL_FLOAT> &close; 
	const QL_MATRIX<QL_FLOAT> &high;
	const QL_MATRIX<QL_FLOAT> &low;
	const QL_MATRIX<QL_FLOAT> &vol;
        int Num1;  
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha60(cfg);
        return str;
    }
}

