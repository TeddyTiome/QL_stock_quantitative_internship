#include "include/qlSim.h"
namespace
{
class alpha86: public AlphaBase
{
    public:
    explicit alpha86(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")),
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        Num1(cfg->getAttributeIntDefault("para1",20)),
	Num2(cfg->getAttributeIntDefault("para2",14)),
	Num3(cfg->getAttributeIntDefault("para3",6)),
	Num4(cfg->getAttributeIntDefault("para4",20))
	{
        adv20.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv20[di].resize(GLOBAL::Instruments.size(),nan);
        }
    }
    void generate(int di) override
    {

            vector<float> rank1(GLOBAL::Instruments.size(),nan);
            vector<float> rank2(GLOBAL::Instruments.size(),nan);

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(Num1,nan);

                for (int j = 0; j < Num1; ++ j)
                {
                    c1[j] = vol[di - delay - j][ii];
                }
                adv20[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                rank2[ii] = float(close[di-delay][ii]-vwap[di-delay][ii]);
            }
        }		

        QL_Oputils::rank(rank2);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
        		vector<float> c2(Num2,nan);
        		vector<float> c3(Num3,nan);
        		vector<float> c4(Num3,nan);
        		vector<float> c5(Num4,nan);

			for (int a= 0; a<Num4;++a){
				for (int b=0;b<Num3;++b){
					for (int c=0; c<Num2;++c){
						c2[c]=adv20[di-delay-c-b-a][ii];
					}
					c3[b] = QL_Oputils::sum(c2);
					c4[b] = close[di-delay-a-b][ii];
				}
				c5[a]=QL_Oputils::corr(c3,c4);
			}
            QL_Oputils::rank(c5);

		rank1[ii] = float(c5[0]);

		if (rank1[ii]<rank2[ii])
            {alpha[ii]=-1;}
		else 
            {alpha[ii]=0;}
	
        }
    }
        return;
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
	const QL_MATRIX<QL_FLOAT> &vol;
	const QL_MATRIX<QL_FLOAT> &vwap;
	const QL_MATRIX<QL_FLOAT> &close;
    vector<vector<float> > adv20;

        int Num1; 
	int Num2;
	int Num3;
	int Num4;
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha86(cfg);
        return str;
    }
}

